#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H

#include <QObject>

class SelectionController : public QObject {
    Q_OBJECT

private:
    SelectionController() {};

public slots:
    void startLocal(bool blackIsComputer, bool whiteIsComputer);
    void startOnline();

signals:
    void updateComputerSides(bool blackIsComputer, bool whiteIsComputer);

public:
    static SelectionController* getInstance() {
        static SelectionController* singleton = nullptr;
        if (!singleton)
            singleton = new SelectionController;
        return singleton;
    }
};

#endif // SELECTIONCONTROLLER_H
