#ifndef COMPUTER_H
#define COMPUTER_H

#include "board.h"

class Computer {
public:
  Computer();

  void clear();

  void update(Coordinate, Unit);
  void remove(Coordinate, Unit);
  Coordinate getBestCoord(Unit);

private:
  int horGroup[BOARD_SIZE][BOARD_SIZE - 4][2];
  int verGroup[BOARD_SIZE - 4][BOARD_SIZE][2];
  int posGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];
  int negGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];

  void fitHorGroup(Coordinate, int, int);
  void fitVerGroup(Coordinate, int, int);
  void fitPosGroup(Coordinate, int, int);
  void fitNegGroup(Coordinate, int, int);

  int horGroupScore(Coordinate);
  int verGroupScore(Coordinate) { return 1; };
  int posGroupScore(Coordinate) { return 1; };
  int negGroupScore(Coordinate) { return 1; };
};

#endif // COMPUTER_H
