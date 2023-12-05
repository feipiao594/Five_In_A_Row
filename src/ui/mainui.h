#ifndef MAINUI_H
#define MAINUI_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>
#include <QLabel>

 #include "piecebutton.h"

class MainUI : public QDialog
{
    Q_OBJECT

private:
    QHBoxLayout *mainLayout;
    QVBoxLayout *black, *white;
    QGridLayout *game_region;
    QWidget *board;
    QLabel *text_black, *text_white;
    PieceButton *pieces[N][N];
    QPushButton *start, *retract_black, *retract_white;

signals:
    void mainUIClosed();

protected:
    void closeEvent(QCloseEvent *event) override {
        emit mainUIClosed();
        QDialog::closeEvent(event);
    }

public slots:


private:
    MainUI();
public:
    static MainUI* getInstance() {
        static MainUI* singleton = nullptr;
        if (!singleton)
            singleton = new MainUI;
        return singleton;
    }
};
#endif // MAINUI_H
