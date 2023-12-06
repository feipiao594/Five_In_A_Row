#include "boardcontroller.h"
#include "../ui/mainui.h"
#include <QDebug>

void BoardController::pieceClicked(int x, int y) {

  qDebug() << "x =" << y << "| y =" << x;

  // 调用后端信息并处理，后端会返回一个信号,该信号会绑定到setDropPieceSuccessful槽函数
  setDropPieceSuccessful();
}

void BoardController::setDropPieceSuccessful() {

  dropPiece(x, y, whoTurn);
  emit updateDropPiece();
}
