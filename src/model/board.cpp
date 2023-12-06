#include "board.h"

Board::Board() {}

Unit Board::getUnit(Coordinate coord) { return units[coord.row][coord.col]; }

void Board::setUnit(Coordinate coord, Unit unit) {
  units[coord.row][coord.col] = unit;
}

void Board::clear() {
  for (int r = 0; r < BOARD_SIZE; r++)
    for (int c = 0; c < BOARD_SIZE; c++)
      units[r][c] = Unit::Empty;
}
