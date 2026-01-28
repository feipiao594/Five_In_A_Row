#include "match.h"

static const Coordinate dirVector[4] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

Match::Match(QObject* parent) : QObject(parent), board(), record() {}

int Match::totalStep() const {
    return (int)record.size();
}

Unit Match::curColor() const {
    return ((totalStep() & 1) == 0) ? Unit::Black : Unit::White;
}

Unit Match::winner() const {
    return winner_;
}

Coordinate Match::latestCoord() const {
    return record.top();
}

QVector<Coordinate> Match::undoList() const {
    return undoList_;
}

bool Match::isOver() const {
    return over_;
}

bool Match::isCoordValid(Coordinate coord) {
    return coord.row >= 0 && coord.row < BOARD_SIZE && coord.col >= 0 && coord.col < BOARD_SIZE;
}

bool Match::isWin(Coordinate baseCoord) const {
    Unit unit = board.getUnit(baseCoord);
    for (int d = 0; d < 4; d++) {
        Coordinate dirCoord = dirVector[d];
        int counter = 1;
        Coordinate coord = baseCoord;
        for (int i = 0; i < 4; i++) {
            coord += dirCoord;
            if (!isCoordValid(coord) || board.getUnit(coord) != unit)
                break;
            counter++;
        }
        coord = baseCoord;
        for (int i = 0; i < 4; i++) {
            coord -= dirCoord;
            if (!isCoordValid(coord) || board.getUnit(coord) != unit)
                break;
            counter++;
        }
        if (counter >= 5)
            return true;
    }
    return false;
}

void Match::drop(Coordinate coord) {
    if (over_)
        return;

    if (board.getUnit(coord) != Unit::Empty) {
        emit onOverlap();
        return;
    }

    Unit unit = curColor();
    board.setUnit(coord, unit);
    record.push(coord);

    // 先更新对局状态，再发信号，避免上层在 onDropped 时误以为还能继续走下一手（和局时会触发 AI
    // 选点崩溃）。
    if (isWin(coord)) {
        winner_ = unit;
        over_ = true;
    } else if (totalStep() >= BOARD_SIZE * BOARD_SIZE) {
        winner_ = Unit::Empty;
        over_ = true;
    }

    emit onDropped();
    if (over_)
        emit onGameOver();
}

Coordinate Match::undo() {
    Coordinate coord = Coordinate(-1, -1);
    if (record.empty())
        return coord;
    coord = record.pop();
    board.setUnit(coord, Unit::Empty);
    return coord;
}

void Match::blackUndo() {
    if (over_)
        return;
    undoList_.clear();
    if (curColor() == Unit::Black) {
        Coordinate coord = undo();
        if (coord.row >= 0)
            undoList_.push_back(coord);
    }
    Coordinate coord = undo();
    if (coord.row >= 0)
        undoList_.push_back(coord);
    emit onUndoDone();
}

void Match::whiteUndo() {
    if (over_)
        return;
    undoList_.clear();
    if (curColor() == Unit::White) {
        Coordinate coord = undo();
        if (coord.row >= 0)
            undoList_.push_back(coord);
    }
    Coordinate coord = undo();
    if (coord.row >= 0)
        undoList_.push_back(coord);
    emit onUndoDone();
}

void Match::restart() {
    board.clear();
    record.clear();
    undoList_.clear();
    winner_ = Unit::Empty;
    over_ = false;
}
