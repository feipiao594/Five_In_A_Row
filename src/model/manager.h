#ifndef MANAGER_H
#define MANAGER_H

#include <QStack>

#include "board.h"
#include "coordinate.h"
#include "player.h"

class Manager : public QObject
{
    Q_OBJECT

public:

    Manager(bool, bool);

    Player getCurPlayer();

    int getTotalStep();
    Unit getCurColor();
    Unit getWinner();

signals:

    void onOverlap();
    void onDropped();
    void onGameOver();

public slots:

    void drop(Coordinate);
    void blackUndo();
    void whiteUndo();

private:

    Board board;
    Player black, white;
    QStack<Coordinate> record;

    Unit winner;

    bool isCoordValid(Coordinate);
    bool isWin(Coordinate);

    void undo();

    Coordinate compute(Unit);

};

#endif // MANAGER_H
