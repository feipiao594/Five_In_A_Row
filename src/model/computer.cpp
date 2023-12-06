#include "computer.h"

Computer::Computer() { clear(); }

void Computer::clear()
{
    for (int r = 0; r < BOARD_SIZE - 4; r++)
    {
        for (int c = 0; c < BOARD_SIZE; c++)
        {
            verGroup[r][c][0] = verGroup[r][c][1] = 0;
            horGroup[c][r][0] = horGroup[c][r][1] = 0;
        }
        for (int c = 0; c < BOARD_SIZE - 4; c++)
        {
            posGroup[r][c][0] = posGroup[r][c][1] = 0;
            negGroup[r][c][0] = negGroup[r][c][1] = 0;
        }
    }
}

void Computer::fitHorGroup(Coordinate coord, int unit, int weight)
{
    int startCol = std::max(coord.col - 4, 0);
    int endCol = std::min(coord.col, BOARD_SIZE - 4);
    for (int c = startCol; c <= endCol; c++)
        horGroup[coord.row][c][unit] += weight;
}

void Computer::fitVerGroup(Coordinate coord, int unit, int weight)
{
    int startRow = std::max(coord.row - 4, 0);
    int endRow = std::min(coord.row, BOARD_SIZE - 4);
    for (int r = startRow; r <= endRow; r++)
        if (r != coord.row) verGroup[r][coord.col][unit] += weight;
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

int Computer::horGroupScore(Coordinate coord)
{

    int startCol = std::max(coord.col - 4, 0);
    int endCol = std::min(coord.col, BOARD_SIZE - 4);
    for (int c = startCol; c <= endCol; c++)
    {
        horGroup[coord.row][c][0];
    }
}

Coordinate Computer::getBestCoord(Unit unit)
{
    int max = 0;
    Coordinate bestCoord;
    for (int r = 0; r < BOARD_SIZE; r++)
    {
        for (int c = 0; c < BOARD_SIZE; c++)
        {
            Coordinate coord = Coordinate(r, c);
            int score = horGroupScore(coord) + verGroupScore(coord) + posGroupScore(coord) + negGroupScore(coord);
            if (score > max)
            {
                max = score;
                bestCoord = coord;
            }
        }
    }
    return bestCoord;
}
