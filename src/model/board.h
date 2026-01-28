#ifndef BOARD_H
#define BOARD_H

#include "coordinate.h"
#include "resource.h"

class Board {
public:
  Unit units[BOARD_SIZE][BOARD_SIZE];

  Board();

  Unit getUnit(Coordinate) const;
  void setUnit(Coordinate, Unit);

  void clear();
};

#endif // BOARD_H
