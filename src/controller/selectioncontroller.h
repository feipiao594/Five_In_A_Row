#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H

#include <QObject>

class SelectionController : public QObject {
  Q_OBJECT

private:
  SelectionController();

public slots:

public:
  static SelectionController *getInstance() {
    static SelectionController *singleton = nullptr;
    if (!singleton)
      singleton = new SelectionController;
    return singleton;
  }
};

#endif // SELECTIONCONTROLLER_H
