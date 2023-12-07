#ifndef SELECTION_H
#define SELECTION_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>

#include "../model/resource.h"

class SelectionUI : public QDialog {
  Q_OBJECT

private:
  QVBoxLayout *mainLayout;
  QPushButton *personModeButton;
  QPushButton *aiModeButton;

public slots:
  void startGamePerson();
  void startGameAI();
  void showSelection();
  void endProgram() { close(); }

signals:
  void clickedAiMode(Unit color);
  void clickedPersonMode();

private:
  SelectionUI();

public:
  void startGame();
  static SelectionUI *getInstance() {
    static SelectionUI *singleton = nullptr;
    if (!singleton)
      singleton = new SelectionUI;
    return singleton;
  }
};
#endif // SELECTION_H
