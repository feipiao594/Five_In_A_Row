#include "computer.h"

Computer::Computer() {}

void Computer::fitHorGroup(Coordinate coord, int unit, int weight)
{
    int row = coord.row;
    int startCol = std::max(coord.col - 4, 0);
    int endCol = std::min(coord.col, BOARD_SIZE - 4);
    for (int c = startCol; c <= endCol; c++)
        horGroup[row][c][unit] += weight;
}

void Computer::fitVerGroup(Coordinate coord, int unit, int weight)
{
    int col = coord.col;
    int startRow = std::max(coord.row - 4, 0);
    int endRow = std::min(coord.row, BOARD_SIZE - 4);
    for (int r = startRow; r <= endRow; r++)
        if (r != coord.row) verGroup[r][col][unit] += weight;
}

void Computer::fitPosGroup(Coordinate coord, int unit, int weight)
{
    int startRow = std::min(coord.row + 4, BOARD_SIZE - 4) - 4;
    int col = coord.col + coord.row - startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r--)
    {
        if (col >= 0 && col < BOARD_SIZE && r != coord.row)
            posGroup[r][col][unit] += weight;
        col++;
    }
}

void Computer::fitNegGroup(Coordinate coord, int unit, int weight)
{
    int startRow = std::max(coord.row - 4, 0);
    int col = coord.col - coord.row + startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r++)
    {
        if (col >= 0 && col < BOARD_SIZE && r != coord.row)
            negGroup[r][col][unit] += weight;
        col++;
    }
}

void Computer::update(Coordinate coord, Unit unit)
{
    int u = unit == Unit::Black ? 0 : 1;
    fitHorGroup(coord, u, 1);
    fitVerGroup(coord, u, 1);
    fitPosGroup(coord, u, 1);
    fitNegGroup(coord, u, 1);
}

void Computer::remove(Coordinate coord, Unit unit)
{
    int u = unit == Unit::Black ? 0 : 1;
    fitHorGroup(coord, u, -1);
    fitVerGroup(coord, u, -1);
    fitPosGroup(coord, u, -1);
    fitNegGroup(coord, u, -1);
}
