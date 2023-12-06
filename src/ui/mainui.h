#ifndef MAINUI_H
#define MAINUI_H

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "piecebutton.h"

class MainUI : public QDialog {
  Q_OBJECT

private:
  QHBoxLayout *mainLayout;
  QVBoxLayout *black, *white;
  QGridLayout *game_region;
  QWidget *board;
  QLabel *text_black, *text_white;
  PieceButton *pieces[BOARD_SIZE][BOARD_SIZE];
  QPushButton *start, *retract_black, *retract_white;

signals:
  void mainUIClosed();
  void onBlackRetract();
  void onWhiteRetract();

protected:
  void closeEvent(QCloseEvent *event) override {
    emit mainUIClosed();
    QDialog::closeEvent(event);
  }

public slots:
  void showDropPiece();
  void restartGame();
  void onGameOver(Unit color);
  void blackRetract() { emit onBlackRetract(); };
  void whiteRetract() { emit onWhiteRetract(); };

private:
  Unit turnColor;
  void clearPieceColor() {
    for (int i = 0; i < BOARD_SIZE; i++)
      for (int j = 0; j < BOARD_SIZE; j++) {
        pieces[i][j]->clearColor();
      }
  };
  MainUI();

public:
  void clearPiecePos(int x, int y);
  void setPieceColor(std::pair<int, int> pos, Unit color) {
    auto [x, y] = pos;
    pieces[x][y]->setColor(color);
    pieces[x][y]->updateColor();
  }
  void piecePushed(int x, int y);
  static MainUI *getInstance() {
    static MainUI *singleton = nullptr;
    if (!singleton)
      singleton = new MainUI;
    return singleton;
  }
};
#endif // MAINUI_H
