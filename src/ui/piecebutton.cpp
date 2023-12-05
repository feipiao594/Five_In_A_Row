#include "piecebutton.h"
#include "../model/resource.h"

PieceButton::PieceButton(const int n, QWidget *parent) : QPushButton(parent) {
    PieceButton::n = n;
    setFixedSize(QSize(n,n));
    clearColor();
}

void PieceButton::clearColor(){
    color = OCCUPATION::NONE;
    canhover = true;
    updateColor();
};

void PieceButton::setColor(OCCUPATION color){
    PieceButton::color = color;
    canhover = false;
    updateColor();
}

void PieceButton::updateColor() {
    setImage(Resource::getInstance()->color2pixmap(color, false));
}

void PieceButton::setImage(const QPixmap &map){
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(map);
    this->setIconSize(QSize(map.width(),map.height()));
}

bool PieceButton::event(QEvent *event){
    if(canhover){
        switch (event->type()) {
        case QEvent::Enter:
            setImage(Resource::getInstance()->color2pixmap(OCCUPATION::BLACK, true));
            break;
        case QEvent::Leave:
            setImage(Resource::getInstance()->color2pixmap(OCCUPATION::NONE, false));
            break;
        default:
            break;
        }
    }
    return QPushButton::event(event);
}
