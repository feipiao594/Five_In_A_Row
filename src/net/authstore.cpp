#include "authstore.h"

#include <QSettings>

#include "../util/settingspath.h"

static QSettings makeSettings() {
  return QSettings(SettingsPath::iniFilePath(), QSettings::IniFormat);
}

QString AuthStore::serverBaseUrl() {
  auto settings = makeSettings();
  return settings.value("net/serverBaseUrl", "http://127.0.0.1:8080").toString();
}

bool AuthStore::ignoreSslErrors() {
  auto settings = makeSettings();
  return settings.value("net/ignoreSslErrors", false).toBool();
}

void AuthStore::setIgnoreSslErrors(bool ignore) {
  auto settings = makeSettings();
  settings.setValue("net/ignoreSslErrors", ignore);
}

void AuthStore::setServerBaseUrl(const QString& baseUrl) {
  QString v = baseUrl.trimmed();
  if (v.isEmpty())
    v = "http://127.0.0.1:8080";
  if (!v.startsWith("http://") && !v.startsWith("https://"))
    v = "http://" + v;
  while (v.endsWith('/'))
    v.chop(1);
  auto settings = makeSettings();
  settings.setValue("net/serverBaseUrl", v);
}

bool AuthStore::isLoggedIn() { return !refreshToken().isEmpty(); }

QString AuthStore::username() {
  auto settings = makeSettings();
  return settings.value("auth/username", "").toString();
}

QString AuthStore::accessToken() {
  auto settings = makeSettings();
  return settings.value("auth/accessToken", "").toString();
}

QString AuthStore::refreshToken() {
  auto settings = makeSettings();
  return settings.value("auth/refreshToken", "").toString();
}

void AuthStore::saveLogin(const QString& username, const QString& accessToken,
                          const QString& refreshToken) {
  auto settings = makeSettings();
  settings.setValue("auth/username", username);
  settings.setValue("auth/accessToken", accessToken);
  settings.setValue("auth/refreshToken", refreshToken);
}

void AuthStore::clearLogin() {
  auto settings = makeSettings();
  settings.remove("auth/username");
  settings.remove("auth/accessToken");
  settings.remove("auth/refreshToken");
}
