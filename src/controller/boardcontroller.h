#ifndef BOARDCONTROLLER_H
#define BOARDCONTROLLER_H

#include <QObject>
#include <utility>

#include "../model/manager.h"
#include "../model/resource.h"

class BoardController : public QObject {
  Q_OBJECT

  int current_drop_x;
  int current_drop_y;
  Unit current_drop_color;
  Unit whoTurn;

signals:
  void updateDropPiece();
  void updateWinner(Unit);
  void updateUndo(int x, int y);
public slots:
  void onGameOverController() {
    emit updateWinner(Manager::getInstance()->getWinner());
  }
  void onDroppedController() {
    current_drop_x = Manager::getInstance()->getLatestCoord().col;
    current_drop_y = Manager::getInstance()->getLatestCoord().row;
    current_drop_color = Manager::getInstance()->getCurColor();
    whoTurn = current_drop_color == Unit::Black ? Unit::White : Unit::Black;
    setDropPieceSuccessful();
  }

  void onUndoDoneController() {
    whoTurn = Manager::getInstance()->getCurColor();
    auto list = Manager::getInstance()->getUndoList();
    for (auto pos : list)
      emit updateUndo(pos.col, pos.row);
  }

public:
  void setDropPieceSuccessful();
  void pieceClicked(int x, int y);
  BoardController() : current_drop_x(-1), current_drop_y(-1) {
    whoTurn = Unit::Black;
  };
  void dropPiece(int x, int y, Unit color) {
    current_drop_x = x;
    current_drop_y = y;
    current_drop_color = color;
    whoTurn = color == Unit::Black ? Unit::White : Unit::Black;
  }

  void restartBoardController() { whoTurn = Unit::Black; }

  Unit getDropPieceColor() { return current_drop_color; }
  Unit getWhoTurn() { return whoTurn; }

  std::pair<int, int> getDropPiecePos() {
    return std::make_pair(current_drop_x, current_drop_y);
  }

  static BoardController *getInstance() {
    static BoardController *singleton = nullptr;
    if (!singleton)
      singleton = new BoardController;
    return singleton;
  }
};

#endif // BOARDCONTROLLER_H
