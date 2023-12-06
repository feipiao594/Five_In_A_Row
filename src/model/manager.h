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

signals:

    void onOverlap();
    void onBlackTurn();
    void onWhiteTurn();
    void onBlackWin();
    void onWhiteWin();
    void onTie();

public slots:

    void drop(Coordinate);
    void blackUndo();
    void whiteUndo();

private:

    Board board;
    Player black, white;
    QStack<Coordinate> record;

    bool isCoordValid(Coordinate);
    bool isWin(Coordinate);

    void undo();

    Coordinate compute(Unit);

};

#endif // MANAGER_H
