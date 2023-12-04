#include "ui.h"
#include "record.h"

// 下标检查，如果越界返回N值特殊处
#define check(a) ((a >= 0 && a < N) ? a : N)
// 求反方向
#define opposite(a) (a % 2 ? a - 1 : a + 1)
// 求所在连线
#define line(a) (a / 2)
// 下一种颜色
#define next(a) (a % 2 + 1)

// x坐标偏移量
constexpr int x_offset(enum DIRECTION direction)
{
    switch (direction)
    {
    case UP:
        return 0;
    case DOWN:
        return 0;
    case RIGHT:
        return +1;
    case LEFT:
        return -1;
    case UP_RIGHT:
        return +1;
    case DOWN_LEFT:
        return -1;
    case UP_LEFT:
        return -1;
    case DOWN_RIGHT:
        return +1;
    }
}

// y坐标偏移量
constexpr int y_offset(enum DIRECTION direction)
{
    switch (direction)
    {
    case UP:
        return +1;
    case DOWN:
        return -1;
    case RIGHT:
        return 0;
    case LEFT:
        return 0;
    case UP_RIGHT:
        return +1;
    case DOWN_LEFT:
        return -1;
    case UP_LEFT:
        return +1;
    case DOWN_RIGHT:
        return -1;
    }
}

Piece::Piece()
    : x(N + 1), y(N + 1), occupation(NONE), nears{NONE}, parent(&board) {}

bool Piece::move(enum OCCUPATION colour)
{
    if (occupation && colour)
    {
        return false;
    }
    occupation = colour;

    parent->pieces[check(x_offset(UP) + x)][check(y_offset(UP) + y)].setNear(DOWN, colour);
    parent->pieces[check(x_offset(DOWN) + x)][check(y_offset(DOWN) + y)].setNear(UP, colour);
    parent->pieces[check(x_offset(RIGHT) + x)][check(y_offset(RIGHT) + y)].setNear(LEFT, colour);
    parent->pieces[check(x_offset(LEFT) + x)][check(y_offset(LEFT) + y)].setNear(RIGHT, colour);
    parent->pieces[check(x_offset(UP_RIGHT) + x)][check(y_offset(UP_RIGHT) + y)].setNear(DOWN_LEFT, colour);
    parent->pieces[check(x_offset(DOWN_LEFT) + x)][check(y_offset(DOWN_LEFT) + y)].setNear(UP_RIGHT, colour);
    parent->pieces[check(x_offset(UP_LEFT) + x)][check(y_offset(UP_LEFT) + y)].setNear(DOWN_RIGHT, colour);
    parent->pieces[check(x_offset(DOWN_RIGHT) + x)][check(y_offset(DOWN_RIGHT) + y)].setNear(UP_LEFT, colour);

    return true;
}

void Piece::setNear(enum DIRECTION direction, enum OCCUPATION colour)
{
    nears[direction] = colour;
    // 如果两端棋子相同，触发flag
    if (nears[opposite(direction)] == colour)
    {
        // 三颗棋子成线，触发胜负判定
        if (colour && occupation == colour)
            parent->winJudge(this, (enum LINE)line(direction));
    }
}

Board::Board()
    : current(FIRST), last_move{nullptr}
{
    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++)
        {
            pieces[i][j].x = i;
            pieces[i][j].y = j;
            //if (i == N || j == N)
            //    pieces[i][j].lock.lock();
        }
}

void Board::clear()
{

    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++)
        {
            pieces[i][j].move(NONE);
            emit moved(i, j, NONE);
        }
    last_move[BLACK] = nullptr;
    last_move[WHITE] = nullptr;
    current = FIRST;
}

void Board::move(int x, int y)
{

    if (pieces[x][y].move(current))
    {
        emit moved(x, y, current);
        record.push(x, y, current);
        last_move[current] = &pieces[x][y];
        current = (enum OCCUPATION)next(current);
    }
    else
        emit error(current);
}

void Board::retract(enum OCCUPATION player)
{

    if (!last_move[player] || !last_move[player]->occupation)
    {
        //lock.unlock();
        return;
    }
    if (current == player)
    {
        last_move[next(player)]->move(NONE);
        emit moved(last_move[next(player)]->x, last_move[next(player)]->y, NONE);
        record.pop((enum OCCUPATION)next(player));
    }
    last_move[player]->move(NONE);
    emit moved(last_move[player]->x, last_move[player]->y, NONE);
    current = player;
    record.pop(player);
}

void Board::winJudge(Piece *piece, enum LINE line)
{
    enum OCCUPATION colour = piece->occupation;
    int amount = 1;
    int offset_x = x_offset((enum DIRECTION)(line * 2)), offset_y = y_offset((enum DIRECTION)(line * 2));
    int x1, x2, y1, y2;

    x1 = x2 = piece->x;
    y1 = y2 = piece->y;

    while (pieces[x1 = check(x1 + offset_x)][y1 = check(y1 + offset_y)].occupation == colour)
        amount++;

    while (pieces[x2 = check(x2 - offset_x)][y2 = check(y2 - offset_y)].occupation == colour)
        amount++;

    if (amount >= WIN_POINT)
    {
        record.save();
        emit win(colour);
    }
}
