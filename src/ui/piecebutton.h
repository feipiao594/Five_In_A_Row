#ifndef PIECEBUTTON_H
#define PIECEBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QEvent>

#include "../model/resource.h"

class PieceButton : public QPushButton {
    Q_OBJECT

    OCCUPATION color;
    bool canhover = true;
    int n;
    int x;
    int y;

public:
    PieceButton(const int _x, const int _y, const int n, QWidget *parent = nullptr);
    void setColor(OCCUPATION color);
    void clearColor();

protected:
    bool event(QEvent *event) override;

public slots:
    void updateColor();
    void buttonPushed();
private:
    void setImage(const QPixmap &map);
};


#endif // PIECEBUTTON_H
