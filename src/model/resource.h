#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>
#include <QObject>
#include <QPixmap>
#include <QPainter>

#include "../ui/selectionui.h"

enum class OCCUPATION
{
    BLACK = -1,
    NONE = 0,
    WHITE = 1
};

class Resource : public QObject {
    Q_OBJECT

private:
    const QString whitePieceUrl = ":/res/whitePiece.png";
    const QString blackPieceUrl = ":/res/blackPiece.png";
    const QString nonePieceUrl = ":/res/nonePiece.png";

    QPixmap whitePiece;
    QPixmap blackPiece;
    QPixmap nonePiece;
    QPixmap whiteTransparentPiece;
    QPixmap blackTransparentPiece;

private:
    QPixmap getPiecePixMap(const QString &url){
        QPixmap map;
        if(!map.load(url))
            qDebug()<<"Photo load failed!";
        return map;
    }

    Resource(){
        setParent(SelectionUI::getInstance());
        whitePiece = getPiecePixMap(whitePieceUrl);
        blackPiece = getPiecePixMap(blackPieceUrl);
        nonePiece = changeOpacity(getPiecePixMap(nonePieceUrl),0);
        whiteTransparentPiece = changeOpacity(whitePiece, 100);
        blackTransparentPiece = changeOpacity(blackPiece, 100);
    };
public:
    static Resource* getInstance() {
        static Resource* singleton = nullptr;
        if (!singleton)
            singleton = new Resource;
        return singleton;
    }
    QPixmap changeOpacity(const QPixmap &map,int opacity){
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
    QPixmap color2pixmap(OCCUPATION color, bool isTransparent) {
        switch(color){
        case OCCUPATION::BLACK:
            if(isTransparent)
                return blackTransparentPiece;
            return blackPiece;
        case OCCUPATION::WHITE:
            if(isTransparent)
                return whiteTransparentPiece;
            return whitePiece;
        case OCCUPATION::NONE:
            return nonePiece;
        }
    }
};

#endif // RESOURCE_H
