#ifndef BOARDCONTROLLER_H
#define BOARDCONTROLLER_H

#include <QObject>
#include <utility>

#include "../model/resource.h"

class BoardController : public QObject {
  Q_OBJECT

  int current_drop_x;
  int current_drop_y;
  Unit current_drop_color;
  Unit whoTurn;

signals:
  void updateDropPiece();
public slots:
  void setDropPieceSuccessful();

public:
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
