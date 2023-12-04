#include "record.h"

#define next(a) (a % 2 + 1)
#define print1(a) #a
#define print(a) print1(a)

Record::Record()
    : current{0}, board(nullptr) {}

void Record::push(int x, int y, enum OCCUPATION colour)
{

    record[colour][current[colour]].x = x;
    record[colour][current[colour]].y = y;

    current[colour]++;
}

void Record::pop(enum OCCUPATION colour)
{

    current[colour]--;
}

void Record::save()
{
    int i = 0, j = 0;
    FILE *file = fopen("data", "a+");

    fprintf(file, "##%s\n", print(FIRST));
    while (i < current[FIRST] && j < current[next(FIRST)])
    {
        fprintf(file, "%d %d\n", record[FIRST][i].x, record[FIRST][i].y);
        fprintf(file, "%d %d\n", record[next(FIRST)][j].x, record[next(FIRST)][j].y);
        i = ++j;
    }
    if (i <= current[FIRST])
        fprintf(file, "%d %d\n", record[FIRST][i].x, record[FIRST][i].y);
}
