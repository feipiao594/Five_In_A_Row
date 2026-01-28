#ifndef RESOURCE_H
#define RESOURCE_H

#include <QDebug>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QString>

// #include "../ui/selectionui.h"

// 棋盘大小
#define BOARD_SIZE 15
// 胜利子数
#define WIN_POINT 5
// 先手
#define FIRST BLACK

enum class Unit {
  Empty = 0,
  Black = -1,
  White = 1,
};

class Resource : public QObject {
  Q_OBJECT

private:
  const QString whitePieceUrl = ":/res/whitePiece.png";
  const QString blackPieceUrl = ":/res/blackPiece.png";
  const QString emptyPieceUrl = ":/res/emptyPiece.png";

  QPixmap whitePiece;
  QPixmap blackPiece;
  QPixmap emptyPiece;
  QPixmap whiteTransparentPiece;
  QPixmap blackTransparentPiece;

public:
  const QString boardUrl = ":/res/board.png";

private:
  QPixmap getPiecePixMap(const QString &url) {
    QPixmap map;
    if (!map.load(url))
      qDebug() << "Photo load failed!";
    return map;
  }

  Resource() {
    whitePiece = getPiecePixMap(whitePieceUrl);
    blackPiece = getPiecePixMap(blackPieceUrl);
    emptyPiece = changeOpacity(getPiecePixMap(emptyPieceUrl), 0);
    whiteTransparentPiece = changeOpacity(whitePiece, 100);
    blackTransparentPiece = changeOpacity(blackPiece, 100);
  };

public:
  static Resource *getInstance() {
    static Resource *singleton = nullptr;
    if (!singleton)
      singleton = new Resource;
    return singleton;
  }
  QPixmap changeOpacity(const QPixmap &map, int opacity) {
    QPixmap temp(map.size());
    temp.fill(Qt::transparent);
    QPainter p1(&temp);
    p1.setCompositionMode(QPainter::CompositionMode_Source);
    p1.drawPixmap(0, 0, map);
    p1.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p1.fillRect(temp.rect(), QColor(0, 0, 0, opacity));
    p1.end();
    return temp;
  }
  QPixmap color2pixmap(Unit color, bool isTransparent) {
    switch (color) {
    case Unit::Black:
      if (isTransparent)
        return blackTransparentPiece;
      return blackPiece;
    case Unit::White:
      if (isTransparent)
        return whiteTransparentPiece;
      return whitePiece;
    case Unit::Empty:
      return emptyPiece;
    }
    return emptyPiece;
  }
};

#endif // RESOURCE_H
