#ifndef BOARD_H
#define BOARD_H

#include <QObject>

// 棋子颜色
enum OCCUPATION
{
    NONE = 0,
    BLACK,
    WHITE
};
// 方向
enum DIRECTION
{
    UP = 0,
    DOWN,
    RIGHT,
    LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    UP_LEFT,
    DOWN_RIGHT
};
// 连线
enum LINE
{
    Y = 0,
    X,
    LEAN,
    COUNTER
};
// 棋盘大小
#define N 19
// 胜利子数
#define WIN_POINT 5
// 先手
#define FIRST BLACK

class Board; // 前置声明

class Piece
{
public:
    int x, y;                   // 位置坐标
    enum OCCUPATION occupation; // 颜色
    enum OCCUPATION nears[8];   // 周围棋子情况
    Board *parent;

public:
    void setNear(enum DIRECTION, enum OCCUPATION); // 修改nears的值
    bool move(enum OCCUPATION);                    // 落子

    Piece();
};

class Board : public QObject
{
    Q_OBJECT

public:
    Piece pieces[N + 1][N + 1]; // pieces[N][y]和[x][N]处理越界情况
private:
    // 决胜子
    enum OCCUPATION current; // 当前选手
    Piece *last_move[3];     // 落子记录
    //QMutex lock;

public slots:
    void move(int, int); // 落子
    void blackRetract() { retract(BLACK); }
    void whiteRetract() { retract(WHITE); }

public:
    void winJudge(Piece *, enum LINE);
    void retract(enum OCCUPATION); // 悔棋
    void clear();

    Board();
signals:
    void moved(int, int, enum OCCUPATION); // 棋子被操作
    void win(enum OCCUPATION);
    void error(enum OCCUPATION);
} extern board;

#endif
