#ifndef MENUUI_H
#define MENUUI_H

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QRadioButton>

class MenuUI : public QDialog {
  Q_OBJECT

private:
  QVBoxLayout *mainLayout;
  QLabel *title;

  QGroupBox *modeGroup;
  QRadioButton *localModeRadio;
  QRadioButton *onlineModeRadio;

  QGroupBox *roomGroup;
  QComboBox *blackType;
  QComboBox *whiteType;

  QPushButton *startButton;
  QPushButton *settingsButton;
  QPushButton *exitButton;

public slots:
  void startGame();
  void showMenu();
  void endProgram() { close(); }
  void onModeChanged();
  void onStartClicked();
  void onSettingsClicked();

signals:
  void startLocal(bool blackIsComputer, bool whiteIsComputer);
  void startOnline();

private:
  MenuUI();

public:
  static MenuUI *getInstance() {
    static MenuUI *singleton = nullptr;
    if (!singleton)
      singleton = new MenuUI;
    return singleton;
  }
};

#endif // MENUUI_H

