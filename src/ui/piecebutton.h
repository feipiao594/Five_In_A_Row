#ifndef PIECEBUTTON_H
#define PIECEBUTTON_H

#include <QEvent>
#include <QPainter>
#include <QPushButton>

#include "../model/resource.h"

class PieceButton : public QPushButton {
  Q_OBJECT

  Unit color;
  bool canhover = true;
  int n;
  int x;
  int y;

public:
  PieceButton(const int _x, const int _y, const int n,
              QWidget *parent = nullptr);
  void updateColor();
  void setColor(Unit color);
  void clearColor();
  void stopUsing() { canhover = false; }

protected:
  bool event(QEvent *event) override;

signals:
  void clickPos(int x, int y);

public slots:
  void buttonPushed();

private:
  void setImage(const QPixmap &map);
};

#endif // PIECEBUTTON_H
