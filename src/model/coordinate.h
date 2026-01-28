#ifndef COORDINATE_H
#define COORDINATE_H

struct Coordinate {
    int row, col;

    Coordinate();
    Coordinate(int, int);

    void operator+=(const Coordinate&);
    void operator-=(const Coordinate&);
};

#endif // COORDINATE_H
