#ifndef MATCH_H
#define MATCH_H

#include <QObject>
#include <QStack>

#include "board.h"
#include "coordinate.h"
#include "resource.h"

// Match：对局核心（棋盘状态 + 规则）。
// 只做：落子/胜负/悔棋/重开；不关心玩家输入来源、不做回合调度。
class Match : public QObject {
    Q_OBJECT

public:
    explicit Match(QObject* parent = nullptr);

    Unit curColor() const;
    Unit winner() const;
    Coordinate latestCoord() const;
    QVector<Coordinate> undoList() const;
    bool isOver() const;

    const Board& boardView() const {
        return board;
    }

signals:
    void onOverlap();
    void onDropped();
    void onGameOver();
    void onUndoDone();

public slots:
    void drop(Coordinate coord);
    void blackUndo();
    void whiteUndo();
    void restart();

private:
    int totalStep() const;
    static bool isCoordValid(Coordinate coord);
    bool isWin(Coordinate baseCoord) const;
    Coordinate undo();

    Board board;
    QStack<Coordinate> record;

    Unit winner_ = Unit::Empty;
    bool over_ = false;
    QVector<Coordinate> undoList_;
};

#endif // MATCH_H
