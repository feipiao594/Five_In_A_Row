#ifndef SELECTION_H
#define SELECTION_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>

class SelectionUI : public QDialog {
  Q_OBJECT

private:
  QVBoxLayout *mainLayout;
  QPushButton *personModeButton;
  QPushButton *aiModeButton;

public slots:
  void startGame();
  void startGameAI();
  void showSelection();
  void endProgram() { close(); }

signals:
  void clickedAiMode();
  void clickedPersonMode();

private:
  SelectionUI();

public:
  static SelectionUI *getInstance() {
    static SelectionUI *singleton = nullptr;
    if (!singleton)
      singleton = new SelectionUI;
    return singleton;
  }
};
#endif // SELECTION_H
