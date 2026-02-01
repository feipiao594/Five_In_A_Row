#include "authapi.h"
#include "authstore.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslError>
#include <QUrl>

static QUrl apiUrl(const QString& path) {
  QString base = AuthStore::serverBaseUrl();
  if (base.endsWith('/'))
    base.chop(1);
  QString p = path;
  if (!p.startsWith('/'))
    p = "/" + p;
  return QUrl(base + p);
}

static bool ensureHttpUrlOrFail(const QUrl& url, const std::function<void(const QString&)>& fail) {
  if (!url.isValid()) {
    fail("服务器地址无效，请在设置中检查“服务器地址”");
    return false;
  }
  const auto scheme = url.scheme().toLower();
  if (scheme != "http" && scheme != "https") {
    fail("服务器地址必须以 http:// 或 https:// 开头");
    return false;
  }
  return true;
}

AuthApi::AuthApi(QObject* parent) : QObject(parent), nam_(new QNetworkAccessManager(this)) {
  connect(nam_, &QNetworkAccessManager::sslErrors, this,
          [](QNetworkReply* reply, const QList<QSslError>& errors) {
            Q_UNUSED(errors);
            if (AuthStore::ignoreSslErrors() && reply) {
              reply->ignoreSslErrors();
            }
          });
}

AuthApi* AuthApi::getInstance() {
  static AuthApi* singleton = nullptr;
  if (!singleton)
    singleton = new AuthApi();
  return singleton;
}

void AuthApi::registerAndLogin(const QString& username, const QString& password) {
  QNetworkRequest req(apiUrl("/api/v1/auth/register"));
  if (!ensureHttpUrlOrFail(req.url(), [this](const QString& m) { emit registerFailed(m); }))
    return;
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject body;
  body["username"] = username;
  body["password"] = password;
  auto reply = nam_->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));

  connect(reply, &QNetworkReply::finished, this, [this, reply, username, password]() {
    const auto httpStatus =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray bytes = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
      emit registerFailed(QString("网络错误：%1").arg(reply->errorString()));
      return;
    }

    if (httpStatus < 200 || httpStatus >= 300) {
      QJsonParseError pe;
      auto doc = QJsonDocument::fromJson(bytes, &pe);
      if (pe.error == QJsonParseError::NoError && doc.isObject()) {
        const auto msg =
            doc.object().value("error").toObject().value("message").toString();
        emit registerFailed(msg.isEmpty() ? "注册失败" : msg);
      } else {
        emit registerFailed("注册失败");
      }
      return;
    }

    // 注册成功后直接登录。
    login(username, password);
  });
}

void AuthApi::login(const QString& username, const QString& password) {
  QNetworkRequest req(apiUrl("/api/v1/auth/login"));
  if (!ensureHttpUrlOrFail(req.url(), [this](const QString& m) { emit loginFailed(m); }))
    return;
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject body;
  body["username"] = username;
  body["password"] = password;
  auto reply = nam_->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const auto httpStatus =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray bytes = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
      emit loginFailed(QString("网络错误：%1").arg(reply->errorString()));
      return;
    }

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(bytes, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
      emit loginFailed("服务器返回格式错误");
      return;
    }

    auto obj = doc.object();
    if (httpStatus < 200 || httpStatus >= 300) {
      const auto errorObj = obj.value("error").toObject();
      const auto msg = errorObj.value("message").toString("登录失败");
      emit loginFailed(msg);
      return;
    }

    const auto u = obj.value("username").toString();
    const auto accessToken = obj.value("accessToken").toString();
    const auto refreshToken = obj.value("refreshToken").toString();

    if (u.isEmpty() || accessToken.isEmpty() || refreshToken.isEmpty()) {
      emit loginFailed("登录返回缺少字段");
      return;
    }

    AuthStore::saveLogin(u, accessToken, refreshToken);
    emit loginSucceeded(u);
  });
}

void AuthApi::logout() {
  const auto refreshToken = AuthStore::refreshToken();
  if (refreshToken.isEmpty()) {
    AuthStore::clearLogin();
    emit logoutFinished(true, "已退出登录");
    return;
  }

  QNetworkRequest req(apiUrl("/api/v1/auth/logout"));
  if (!ensureHttpUrlOrFail(req.url(), [this](const QString& m) { emit logoutFinished(false, m); })) {
    AuthStore::clearLogin();
    return;
  }
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject body;
  body["refreshToken"] = refreshToken;
  auto reply = nam_->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const auto httpStatus =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray bytes = reply->readAll();
    const auto err = reply->error();
    const auto errStr = reply->errorString();
    reply->deleteLater();

    // 无论服务端是否成功，都清理本地凭证，避免卡死在“已登录”状态。
    AuthStore::clearLogin();

    if (err != QNetworkReply::NoError) {
      emit logoutFinished(false, QString("网络错误：%1").arg(errStr));
      return;
    }

    if (httpStatus < 200 || httpStatus >= 300) {
      QJsonParseError pe;
      auto doc = QJsonDocument::fromJson(bytes, &pe);
      if (pe.error == QJsonParseError::NoError && doc.isObject()) {
        const auto msg =
            doc.object().value("error").toObject().value("message").toString();
        emit logoutFinished(false, msg.isEmpty() ? "退出登录失败" : msg);
      } else {
        emit logoutFinished(false, "退出登录失败");
      }
      return;
    }

    emit logoutFinished(true, "已退出登录");
  });
}

void AuthApi::checkSession() {
  if (sessionCheckInFlight_)
    return;

  const auto refreshToken = AuthStore::refreshToken();
  if (refreshToken.isEmpty()) {
    emit sessionChecked(false, "未登录");
    return;
  }

  sessionCheckInFlight_ = true;

	  const auto accessToken = AuthStore::accessToken();
	  auto doRefresh = [this](const QString &rt) {
	    QNetworkRequest req(apiUrl("/api/v1/auth/refresh"));
	    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	    QJsonObject body;
	    body["refreshToken"] = rt;
	    auto reply = nam_->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
	
	    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
	      const auto httpStatus =
	          reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	      const QByteArray bytes = reply->readAll();
	      const auto netErr = reply->error();
	      const auto netErrStr = reply->errorString();
	      reply->deleteLater();

	      // 注意：Qt 在 HTTP 4xx/5xx 时可能也会设置 netErr（例如 401 -> AuthenticationRequiredError）。
	      // 这里以 httpStatus 为准：若 httpStatus==0 才认为是“没拿到 HTTP 响应”的网络错误。
	      if (httpStatus == 0 && netErr != QNetworkReply::NoError) {
	        sessionCheckInFlight_ = false;
	        emit sessionChecked(false, QString("无法刷新登录：%1").arg(netErrStr));
	        return;
	      }

	      QJsonObject obj;
	      QJsonParseError err;
	      auto doc = QJsonDocument::fromJson(bytes, &err);
	      if (err.error == QJsonParseError::NoError && doc.isObject())
	        obj = doc.object();

	      if (httpStatus < 200 || httpStatus >= 300) {
	        AuthStore::clearLogin();
	        const auto msg = obj.value("error").toObject().value("message").toString();
	        sessionCheckInFlight_ = false;
	        emit sessionChecked(false,
	                            msg.isEmpty() ? "登录已失效，请重新登录" : msg);
	        return;
	      }

	      const auto at = obj.value("accessToken").toString();
	      const auto newRt = obj.value("refreshToken").toString();
	      if (at.isEmpty()) {
	        AuthStore::clearLogin();
	        sessionCheckInFlight_ = false;
	        emit sessionChecked(false, "登录已失效，请重新登录");
	        return;
	      }

	      const auto u = AuthStore::username();
	      AuthStore::saveLogin(u, at,
	                           newRt.isEmpty() ? AuthStore::refreshToken() : newRt);
	      sessionCheckInFlight_ = false;
	      emit sessionChecked(true, QString("已登录：%1").arg(AuthStore::username()));
	    });
	  };

  if (accessToken.isEmpty()) {
    doRefresh(refreshToken);
    return;
  }

  QNetworkRequest req(apiUrl("/api/v1/auth/me"));
  if (!ensureHttpUrlOrFail(req.url(), [this](const QString& m) {
        sessionCheckInFlight_ = false;
        emit sessionChecked(true, m);
      })) {
    return;
  }
  req.setRawHeader("Authorization", QByteArray("Bearer ") + accessToken.toUtf8());
  auto reply = nam_->get(req);

	  connect(reply, &QNetworkReply::finished, this,
	          [this, reply, refreshToken, doRefresh]() mutable {
	            const auto httpStatus =
	                reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	            const QByteArray bytes = reply->readAll();
	            const auto netErr = reply->error();
	            const auto netErrStr = reply->errorString();
	            reply->deleteLater();

	            // httpStatus==0 -> 没有拿到 HTTP 响应（DNS/网络/SSL 等）。
	            if (httpStatus == 0 && netErr != QNetworkReply::NoError) {
	              sessionCheckInFlight_ = false;
	              emit sessionChecked(false, QString("无法校验登录：%1").arg(netErrStr));
	              return;
	            }

	            if (httpStatus >= 200 && httpStatus < 300) {
	              QJsonParseError pe;
	              auto doc = QJsonDocument::fromJson(bytes, &pe);
	              if (pe.error == QJsonParseError::NoError && doc.isObject()) {
	                const auto u = doc.object().value("username").toString();
	                if (!u.isEmpty())
	                  AuthStore::saveLogin(u, AuthStore::accessToken(),
	                                       AuthStore::refreshToken());
	              }
	              sessionCheckInFlight_ = false;
	              emit sessionChecked(true, QString("已登录：%1").arg(AuthStore::username()));
	              return;
	            }

	            // access token 无效/过期：尝试 refresh（401/403）
	            if (httpStatus == 401 || httpStatus == 403) {
	              doRefresh(refreshToken);
	              return;
	            }

	            // 其它 HTTP 错误：不清理本地凭证（避免偶发服务端错误把用户强制登出），仅提示。
	            sessionCheckInFlight_ = false;
	            emit sessionChecked(false,
	                                QString("无法校验登录：HTTP %1").arg(httpStatus));
	          });
}
