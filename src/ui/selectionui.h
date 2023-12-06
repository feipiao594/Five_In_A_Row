#ifndef SELECTION_H
#define SELECTION_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>

class SelectionUI : public QDialog
{
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;
    QPushButton *aiModeButton;
    QPushButton *personModeButton;

public slots:
    void startGame();
    void showSelection();
    void endProgram();

signals:
    void clickedAiMode();
    void clickedPersonMode();

private:
    SelectionUI();

public:
    static SelectionUI* getInstance() {
        static SelectionUI* singleton = nullptr;
        if (!singleton)
            singleton = new SelectionUI;
        return singleton;
    }
};
#endif // SELECTION_H
