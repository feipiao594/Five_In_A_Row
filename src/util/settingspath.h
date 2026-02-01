#ifndef SETTINGSPATH_H
#define SETTINGSPATH_H

#include <QString>

class SettingsPath {
public:
  // Returns the ini file path used by the app.
  //
  // Priority:
  // 1) env `FIVE_IN_A_ROW_SETTINGS` (absolute or relative)
  // 2) argv `--settings <path>` (absolute or relative)
  // 3) argv `--profile <name>` -> settings.<name>.ini
  // 4) settings.ini
  //
  // Default behavior:
  // - Prefer writing next to the executable (dev builds).
  // - If the executable directory is not writable (AppImage), fallback to
  //   QStandardPaths::AppConfigLocation.
  static QString iniFilePath();
};

#endif // SETTINGSPATH_H

