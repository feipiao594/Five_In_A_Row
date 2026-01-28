#include "coordinate.h"

Coordinate::Coordinate() {}

Coordinate::Coordinate(int row, int col) : row(row), col(col) {}

void Coordinate::operator+=(const Coordinate& coord) {
    row += coord.row;
    col += coord.col;
}

void Coordinate::operator-=(const Coordinate& coord) {
    row -= coord.row;
    col -= coord.col;
}
