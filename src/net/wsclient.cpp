#include "wsclient.h"

#include "authstore.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSslError>
#include <QUrlQuery>
#include <QUuid>
#include <QWebSocket>

WsClient::WsClient(QObject* parent) : QObject(parent), ws_(new QWebSocket()) {
  ws_->setParent(this);

  connect(ws_, &QWebSocket::connected, this, [this]() {
    connecting_ = false;
    emit connected();
  });
  connect(ws_, &QWebSocket::disconnected, this, [this]() {
    connecting_ = false;
    emit disconnected();
  });
  connect(ws_, &QWebSocket::textMessageReceived, this,
          &WsClient::handleTextMessage);
  connect(ws_, &QWebSocket::sslErrors, this,
          [this](const QList<QSslError>& errors) {
            Q_UNUSED(errors);
            if (AuthStore::ignoreSslErrors()) {
              ws_->ignoreSslErrors();
            }
          });
  connect(ws_, &QWebSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError) {
            connecting_ = false;
            emit errorOccurred(ws_->errorString());
          });
}

WsClient* WsClient::getInstance() {
  static WsClient* singleton = nullptr;
  if (!singleton)
    singleton = new WsClient();
  return singleton;
}

bool WsClient::isConnected() const {
  return ws_->state() == QAbstractSocket::ConnectedState;
}

QUrl WsClient::toWsUrl(const QString& httpBase, const QString& accessToken) {
  QString base = httpBase.trimmed();
  while (base.endsWith('/'))
    base.chop(1);

  QUrl url(base);
  if (url.scheme().toLower() == "http")
    url.setScheme("ws");
  else if (url.scheme().toLower() == "https")
    url.setScheme("wss");
  url.setPath("/ws");

  QUrlQuery q;
  q.addQueryItem("accessToken", accessToken);
  url.setQuery(q);
  return url;
}

void WsClient::openSocket(const QUrl& url) {
  connecting_ = true;
  ws_->open(url);
}

void WsClient::connectIfNeeded() {
  if (isConnected() || connecting_)
    return;

  const auto token = AuthStore::accessToken();
  if (token.isEmpty()) {
    emit errorOccurred("未登录：缺少 accessToken");
    return;
  }

  const auto base = AuthStore::serverBaseUrl();
  const auto url = toWsUrl(base, token);
  if (!url.isValid()) {
    emit errorOccurred("服务器地址无效，请在设置中检查");
    return;
  }
  openSocket(url);
}

void WsClient::disconnectNow() {
  connecting_ = false;
  if (ws_->state() == QAbstractSocket::ConnectedState ||
      ws_->state() == QAbstractSocket::ConnectingState) {
    ws_->close();
  }
}

QString WsClient::sendRequest(const QString& type, const QJsonObject& payload) {
  QJsonObject env;
  env["v"] = 1;
  env["type"] = type;
  const auto reqId = QUuid::createUuid().toString(QUuid::WithoutBraces);
  env["reqId"] = reqId;
  env["payload"] = payload;
  ws_->sendTextMessage(QJsonDocument(env).toJson(QJsonDocument::Compact));
  return reqId;
}

void WsClient::sendEvent(const QString& type, const QJsonObject& payload) {
  QJsonObject env;
  env["v"] = 1;
  env["type"] = type;
  env["payload"] = payload;
  ws_->sendTextMessage(QJsonDocument(env).toJson(QJsonDocument::Compact));
}

void WsClient::handleTextMessage(const QString& text) {
  QJsonParseError err;
  const auto doc = QJsonDocument::fromJson(text.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError || !doc.isObject())
    return;

  const auto obj = doc.object();
  const auto type = obj.value("type").toString();
  if (type.isEmpty())
    return;

  const auto payloadObj = obj.value("payload").toObject();
  if (type.endsWith(".resp")) {
    const auto reqId = obj.value("reqId").toString();
    const bool ok = obj.value("ok").toBool();
    const auto errorObj = obj.value("error").toObject();
    const auto errorMessage = errorObj.value("message").toString();
    const auto errorCode = errorObj.value("code").toString();
    emit responseReceived(type, reqId, ok, payloadObj, errorMessage, errorCode);
    return;
  }

  emit eventReceived(type, payloadObj);
}
