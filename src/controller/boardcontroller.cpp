#include "boardcontroller.h"
#include <QDebug>

void BoardController::pieceClicked(int x, int y) {
    emit boardClicked(x, y);
}

void BoardController::setDropPieceSuccessful() {
    dropPiece(current_drop_x, current_drop_y, whoTurn);
    emit updateDropPiece();
}
