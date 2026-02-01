#ifndef AUTHSTORE_H
#define AUTHSTORE_H

#include <QString>

class AuthStore {
public:
  static QString serverBaseUrl();
  static void setServerBaseUrl(const QString& baseUrl);
  static bool ignoreSslErrors();
  static void setIgnoreSslErrors(bool ignore);

  static bool isLoggedIn();
  static QString username();
  static QString accessToken();
  static QString refreshToken();

  static void saveLogin(const QString& username, const QString& accessToken,
                        const QString& refreshToken);
  static void clearLogin();
};

#endif // AUTHSTORE_H
