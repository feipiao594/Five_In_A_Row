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
public:
    PieceButton(const int n, QWidget *parent = nullptr);
    void setColor(OCCUPATION color);
    void clearColor();

protected:
    bool event(QEvent *event) override;

public slots:
    void updateColor();

private:
    void setImage(const QPixmap &map);
};


#endif // PIECEBUTTON_H
