#ifndef MANAGER_H
#define MANAGER_H

#include <QStack>

#include "board.h"
#include "computer.h"
#include "coordinate.h"
#include "player.h"

class Manager : public QObject {
  Q_OBJECT

public:
  Manager(bool, bool);

  Player getCurPlayer();

    int getTotalStep();
    Unit getCurColor();
    Unit getWinner();
    Coordinate getLatestCoord();
    QVector<Coordinate> getUndoList();

  static Manager *getInstance() {
    static Manager *singleton = nullptr;
    if (!singleton)
      singleton = new Manager(false, false);
    return singleton;
  }

signals:

    void onOverlap();
    void onDropped();
    void onGameOver();
    void onUndoDone();

public slots:

  void drop(Coordinate);
  void blackUndo();
  void whiteUndo();

private:

    Board board;
    Player black, white;
    QStack<Coordinate> record;
    Computer computer;

    Unit winner;
    QVector<Coordinate> undoList;

  bool isCoordValid(Coordinate);
  bool isWin(Coordinate);

    Coordinate undo();

  Coordinate compute(Unit);
};

#endif // MANAGER_H
