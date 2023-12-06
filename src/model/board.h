#ifndef BOARD_H
#define BOARD_H

#include "coordinate.h"

#define BOARD_SIZE 15

enum Unit
{
    Empty = 0,
    Black = -1,
    White = 1,
};

class Board
{
public:

    Unit units[BOARD_SIZE][BOARD_SIZE];

    Board();

    Unit getUnit(Coordinate);
    void setUnit(Coordinate, Unit);

    void clear();
};

#endif // BOARD_H
