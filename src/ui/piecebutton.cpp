#include "piecebutton.h"
#include "../model/resource.h"

PieceButton::PieceButton(const int _x, const int _y, const int n, QWidget *parent)
    : QPushButton(parent), x(_x), y(_y) {
    PieceButton::n = n;
    setStyleSheet("border-image:none");
    setFixedSize(QSize(n, n));
    setFlat(true);
    clearColor();
    connect(this, &QPushButton::clicked, this, &PieceButton::buttonPushed);
}

void PieceButton::clearColor(){
    setImage(Resource::getInstance()->color2pixmap(OCCUPATION::NONE, false));
    color = OCCUPATION::NONE;
    canhover = true;
};

void PieceButton::setColor(OCCUPATION color){
    PieceButton::color = color;
    canhover = false;
    updateColor();
    //wait for api
}

void PieceButton::updateColor() {
    setImage(Resource::getInstance()->color2pixmap(color, false));
}

void PieceButton::buttonPushed() {
    qDebug()<<"x: "<<x<<"\ny: "<<y;
    canhover = false;
    setImage(Resource::getInstance()->color2pixmap(OCCUPATION::BLACK, false));
}

void PieceButton::setImage(const QPixmap &map){
    setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(map);
    this->setIconSize(QSize(n,n));
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
