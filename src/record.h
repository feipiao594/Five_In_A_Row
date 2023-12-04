#ifndef RECORD_H
#define RECORD_H

#include "board.h"

class Record
{
private:
    struct stack
    {
        int x, y;
        struct list *next;
    } record[3][N * N / 2];

    int current[3];
    Board *board;

public:
    void push(int, int, enum OCCUPATION);
    void pop(enum OCCUPATION);
    void save();

    Record();
} extern record;

#endif // RECORD_H
