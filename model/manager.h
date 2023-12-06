#ifndef MANAGER_H
#define MANAGER_H

#include <QStack>

#include "board.h"
#include "coordinate.h"
#include "player.h"

class Manager
{
public:

    Manager(bool, bool);

    int getTotalStep();
    Player getCurPlayer();

    void (*onOverlap)();
    void (*onBlackTurn)();
    void (*onWhiteTurn)();
    void (*onBlackWin)();
    void (*onWhiteWin)();
    void (*onTie)();

    void drop(Coordinate);

    void blackUndo();
    void whiteUndo();

private:

    Board board;
    Player black, white;
    QStack<Coordinate> record;

    bool isCoordValid(Coordinate);
    bool isWin(Coordinate);

    void Undo();

    Coordinate compute(Unit);

};

#endif // MANAGER_H
