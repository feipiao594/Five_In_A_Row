#include "settingspath.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>

static QString appDir() { return QCoreApplication::applicationDirPath(); }

static QString configDir() {
  const auto base =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir().mkpath(base);
  return base;
}

static bool isDirWritable(const QString& dir) {
  if (dir.isEmpty())
    return false;
  QFileInfo fi(dir);
  return fi.exists() && fi.isDir() && fi.isWritable();
}

static QString defaultBaseDir() {
  const auto d = appDir();
  if (isDirWritable(d))
    return d;
  return configDir();
}

static QString toPath(const QString& v) {
  if (v.isEmpty())
    return {};
  QFileInfo fi(v);
  if (fi.isAbsolute())
    return fi.absoluteFilePath();
  return QDir(defaultBaseDir()).absoluteFilePath(v);
}

QString SettingsPath::iniFilePath() {
  const auto env = QProcessEnvironment::systemEnvironment();
  const auto envPath = env.value("FIVE_IN_A_ROW_SETTINGS").trimmed();
  if (!envPath.isEmpty())
    return toPath(envPath);

  const auto args = QCoreApplication::arguments();
  for (int i = 0; i < args.size(); i++) {
    if (args.at(i) == "--settings" && i + 1 < args.size())
      return toPath(args.at(i + 1).trimmed());
  }

  QString profile;
  for (int i = 0; i < args.size(); i++) {
    if (args.at(i) == "--profile" && i + 1 < args.size()) {
      profile = args.at(i + 1).trimmed();
      break;
    }
  }

  const auto base = defaultBaseDir();
  const auto file = profile.isEmpty() ? "settings.ini"
                                      : QString("settings.%1.ini").arg(profile);

  const auto absPath = QDir(base).absoluteFilePath(file);
  QDir().mkpath(QFileInfo(absPath).absolutePath());
  return absPath;
}

