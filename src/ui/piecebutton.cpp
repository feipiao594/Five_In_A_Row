#include "piecebutton.h"
#include "../controller/boardcontroller.h"
#include "../model/resource.h"

PieceButton::PieceButton(const int _x, const int _y, const int n,
                         QWidget *parent)
    : QPushButton(parent), x(_x), y(_y) {
  PieceButton::n = n;
  setStyleSheet("border-image:none");
  setFixedSize(QSize(n, n));
  setFlat(true);
  clearColor();
  connect(this, &QPushButton::clicked, this, &PieceButton::buttonPushed);
}

void PieceButton::clearColor() {
  setImage(Resource::getInstance()->color2pixmap(Unit::Empty, false));
  color = Unit::Empty;
  canhover = true;
};

void PieceButton::setColor(Unit color) {
  PieceButton::color = color;
  canhover = false;
  updateColor();
}

void PieceButton::updateColor() {
  setImage(Resource::getInstance()->color2pixmap(color, false));
}

void PieceButton::buttonPushed() {
  if (canhover && BoardController::getInstance()->getIsYourTurn()) {
    canhover = false;
    BoardController::getInstance()->pieceClicked(x, y);
  }
}

void PieceButton::setImage(const QPixmap &map) {
  setStyleSheet("QPushButton{border:0px;}");
  this->setIcon(map);
  this->setIconSize(QSize(n, n));
}

bool PieceButton::event(QEvent *event) {
  if (canhover && BoardController::getInstance()->getIsYourTurn()) {
    switch (event->type()) {
    case QEvent::Enter:
      setImage(Resource::getInstance()->color2pixmap(
          BoardController::getInstance()->getWhoTurn(), true));
      break;
    case QEvent::Leave:
      setImage(Resource::getInstance()->color2pixmap(Unit::Empty, false));
      break;
    default:
      break;
    }
  }
  return QPushButton::event(event);
}
