#include "computer.h"

Computer::Computer() {}

void Computer::fitHorGroup(Coordinate coord, void(*callback)(int[]))
{
    int row = coord.row;
    int startCol = std::max(coord.col - 4, 0);
    int endCol = std::min(coord.col, BOARD_SIZE - 4);
    for (int c = startCol; c <= endCol; c++) callback(horGroup[row][c]);
}

void Computer::fitVerGroup(Coordinate coord, void(*callback)(int[]))
{
    int col = coord.col;
    int startRow = std::max(coord.row - 4, 0);
    int endRow = std::min(coord.row, BOARD_SIZE - 4);
    for (int r = startRow; r <= endRow; r++)
        if (r != coord.row) callback(verGroup[r][col]);
}

void Computer::fitPosGroup(Coordinate coord, void(*callback)(int[]))
{
    int startRow = std::min(coord.row + 4, BOARD_SIZE - 4) - 4;
    int col = coord.col + coord.row - startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r--)
    {
        if (col >= 0 && col < BOARD_SIZE && r != coord.row)
            callback(posGroup[r][col]);
        col++;
    }
}

void Computer::fitNegGroup(Coordinate coord, void(*callback)(int[]))
{
    int startRow = std::max(coord.row - 4, 0);
    int col = coord.col - coord.row + startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r++)
    {
        if (col >= 0 && col < BOARD_SIZE && r != coord.row)
            callback(negGroup[r][col]);
        col++;
    }
}

void update_callback(int group[]) {  }

void Computer::update(Coordinate coord, Unit unit)
{
    fitHorGroup(coord, update_callback);
    fitVerGroup(coord, update_callback);
    fitPosGroup(coord, update_callback);
    fitNegGroup(coord, update_callback);
}
