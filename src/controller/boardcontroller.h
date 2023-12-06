#ifndef BOARDCONTROLLER_H
#define BOARDCONTROLLER_H

#include <QObject>
#include <utility>

class BoardController : public QObject
{
    Q_OBJECT

    int current_drop_x;
    int current_drop_y;

signals:
    void updateDropPiece();

public:
    BoardController() : current_drop_x(-1), current_drop_y(-1){};
    void dropPiece(int x,int y) {
        current_drop_x = x;
        current_drop_y = y;
    }
    std::pair<int,int> getDropPiece(){
        return std::make_pair(current_drop_x, current_drop_y);
    }

    static BoardController* getInstance() {
        static BoardController* singleton = nullptr;
        if (!singleton)
            singleton = new BoardController;
        return singleton;
    }
};

#endif // BOARDCONTROLLER_H
