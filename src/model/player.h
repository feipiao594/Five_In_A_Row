#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

class Player
{
public:

    Unit unit;
    bool isComputer;

    Player();
    Player(Unit, bool);

};

#endif // PLAYER_H
