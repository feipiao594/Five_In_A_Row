#include "boardcontroller.h"
#include "../ui/mainui.h"
#include <QDebug>

void BoardController::pieceClicked(int x, int y) {
  Manager::getInstance()->drop(Coordinate(y, x));
}

void BoardController::setDropPieceSuccessful() {
  dropPiece(current_drop_x, current_drop_y, whoTurn);
  emit updateDropPiece();
}
