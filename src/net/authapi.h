#ifndef AUTHAPI_H
#define AUTHAPI_H

#include <QObject>

class QNetworkAccessManager;

class AuthApi : public QObject {
  Q_OBJECT

public:
  static AuthApi* getInstance();

  void registerAndLogin(const QString& username, const QString& password);
  void login(const QString& username, const QString& password);
  void checkSession();
  void logout();

signals:
  void loginSucceeded(const QString& username);
  void loginFailed(const QString& message);
  void registerFailed(const QString& message);
  void sessionChecked(bool loggedIn, const QString& message);
  void logoutFinished(bool ok, const QString& message);

private:
  explicit AuthApi(QObject* parent = nullptr);

  QNetworkAccessManager* nam_;
  bool sessionCheckInFlight_ = false;
};

#endif // AUTHAPI_H
