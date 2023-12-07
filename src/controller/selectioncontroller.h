#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H

#include "../model/resource.h"
#include <QObject>

class SelectionController : public QObject {
  Q_OBJECT

private:
  SelectionController(){};

public slots:
  void selectAImode(Unit color);
  void selectPersonmode();

signals:
  void updateAiMode(Unit color);

public:
  static SelectionController *getInstance() {
    static SelectionController *singleton = nullptr;
    if (!singleton)
      singleton = new SelectionController;
    return singleton;
  }
};

#endif // SELECTIONCONTROLLER_H
