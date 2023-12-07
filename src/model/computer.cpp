#include <random>

#include "computer.h"

Computer::Computer() { clear(); }

Computer::Computer(Board &board)
{
    this->board = &board;
    clear();
}

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

void Computer::fitGroup(Coordinate coord, int unit, int weight)
{
    fitHorGroup(coord, unit, 1);
    fitVerGroup(coord, unit, 1);
    fitPosGroup(coord, unit, 1);
    fitNegGroup(coord, unit, 1);
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
    int col = coord.col + coord.row - 4 - startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r--)
    {
        if (col >= 0 && col < BOARD_SIZE - 4 && r != coord.row)
            posGroup[r][col][unit] += weight;
        col++;
    }
}

void Computer::fitNegGroup(Coordinate coord, int unit, int weight)
{
    int startRow = std::max(coord.row - 4, 0);
    int col = coord.col - coord.row + startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r <= endRow; r++)
    {
        if (col >= 0 && col < BOARD_SIZE - 4 && r != coord.row)
            negGroup[r][col][unit] += weight;
        col++;
    }
}

void Computer::update(Coordinate coord, Unit unit)
{
    fitGroup(coord, unit == Unit::Black ? 0 : 1, 1);
}

void Computer::remove(Coordinate coord, Unit unit)
{
    fitGroup(coord, unit == Unit::Black ? 0 : 1, -1);
}

int Computer::count2Score(int counts[2], Unit unit)
{
    if (counts[0] == 0 && counts[1] == 0) return 7;
    if (counts[0] > 0 && counts[1] > 0) return 0;

    int local = unit == Unit::Black ? counts[0] : counts[1];
    int rival = unit == Unit::White ? counts[1] : counts[0];

    if (rival == 0)
    {
        if (local == 1) return 35;
        if (local == 2) return 800;
        if (local == 3) return 15000;
        return 800000;
    }
    else
    {
        if (rival == 1) return 15;
        if (rival == 2) return 400;
        if (rival == 3) return 1800;
        return 100000;
    }
}

int Computer::horGroupScore(Coordinate coord, Unit unit)
{
    int score = 0;
    int startCol = std::max(coord.col - 4, 0);
    int endCol = std::min(coord.col, BOARD_SIZE - 4);
    for (int c = startCol; c <= endCol; c++)
        score += count2Score(horGroup[coord.row][c], unit);
    return score;
}

int Computer::verGroupScore(Coordinate coord, Unit unit)
{
    int score = 0;
    int startRow = std::max(coord.row - 4, 0);
    int endRow = std::min(coord.row, BOARD_SIZE - 4);
    for (int r = startRow; r <= endRow; r++)
        if (r != coord.row) score += count2Score(verGroup[r][coord.col], unit);
    return score;
}

int Computer::posGroupScore(Coordinate coord, Unit unit)
{
    int score = 0;
    int startRow = std::min(coord.row + 4, BOARD_SIZE - 4) - 4;
    int col = coord.col + coord.row - 4 - startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r >= endRow; r--)
    {
        if (col >= 0 && col < BOARD_SIZE - 4 && r != coord.row)
            score += count2Score(posGroup[r][col], unit);
        col++;
    }
    return score;
}

int Computer::negGroupScore(Coordinate coord, Unit unit)
{
    int score = 0;
    int startRow = std::max(coord.row - 4, 0);
    int col = coord.col - coord.row + startRow;
    int endRow = std::max(coord.row - 4, 0);
    for (int r = startRow; r <= endRow; r++)
    {
        if (col >= 0 && col < BOARD_SIZE - 4 && r != coord.row)
            score += count2Score(negGroup[r][col], unit);
        col++;
    }
    return score;
}

Coordinate Computer::getBestCoord(Unit unit)
{
    int max = 0;
    QVector<Coordinate> candidateList;

    for (int r = 0; r < BOARD_SIZE; r++)
    {
        for (int c = 0; c < BOARD_SIZE; c++)
        {
            if (board->units[r][c] != Unit::Empty) continue;

            Coordinate coord = Coordinate(r, c);

            int score = horGroupScore(coord, unit) +
                        verGroupScore(coord, unit) +
                        posGroupScore(coord, unit) +
                        negGroupScore(coord, unit);

            if (score > max)
            {
                max = score;
                candidateList.push_back(coord);
            }
        }
    }

    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(0, candidateList.size());
    int random = distrib(engine);

    return candidateList.at(random);
}
