#ifndef COMPUTER_H
#define COMPUTER_H

#include "board.h"

class Computer {
public:
  Computer();
  Computer(Board &);

  void clear();

  void update(Coordinate, Unit);
  void remove(Coordinate, Unit);
  Coordinate getBestCoord(Unit);
  Coordinate getGameCoord(Unit);

private:
  Board *board;

  int horGroup[BOARD_SIZE][BOARD_SIZE - 4][2];
  int verGroup[BOARD_SIZE - 4][BOARD_SIZE][2];
  int posGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];
  int negGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];

  void fitGroup(Coordinate, int, int);
  void fitHorGroup(Coordinate, int, int);
  void fitVerGroup(Coordinate, int, int);
  void fitPosGroup(Coordinate, int, int);
  void fitNegGroup(Coordinate, int, int);

  static int count2Score(int, int, Unit);
  static int getRandomInt(int);

  int horGroupScore(Coordinate, Unit);
  int verGroupScore(Coordinate, Unit);
  int posGroupScore(Coordinate, Unit);
  int negGroupScore(Coordinate, Unit);

  int selectCandidates(QVector<std::pair<Coordinate, int>>&, Unit);
};

#endif // COMPUTER_H
