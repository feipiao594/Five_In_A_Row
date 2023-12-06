#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H

#include <QObject>

class SelectionController : public QObject
{
    Q_OBJECT
public:
    explicit SelectionController(QObject *parent = nullptr);

signals:

};

#endif // SELECTIONCONTROLLER_H
