#ifndef COMPUTER_H
#define COMPUTER_H

#include "board.h"

class Computer {
public:

    Computer();

    void update(Coordinate, Unit);
    void remove(Coordinate, Unit);

private:

    int horGroup[BOARD_SIZE][BOARD_SIZE - 4][2];
    int verGroup[BOARD_SIZE - 4][BOARD_SIZE][2];
    int posGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];
    int negGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];

    void fitHorGroup(Coordinate, void(*)(int[]));
    void fitVerGroup(Coordinate, void(*)(int[]));
    void fitPosGroup(Coordinate, void(*)(int[]));
    void fitNegGroup(Coordinate, void(*)(int[]));

};

#endif // COMPUTER_H
