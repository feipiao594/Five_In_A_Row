#ifndef COMPUTER_H
#define COMPUTER_H

#include "board.h"

class computer {
public:

    computer();

    void push(Coordinate, Unit);
    void pop(Coordinate, Unit);

private:

    int horGroup[BOARD_SIZE][BOARD_SIZE - 4][2];
    int verGroup[BOARD_SIZE - 4][BOARD_SIZE][2];
    int posGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];
    int negGroup[BOARD_SIZE - 4][BOARD_SIZE - 4][2];



};

#endif // COMPUTER_H
