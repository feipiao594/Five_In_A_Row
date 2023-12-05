#ifndef MAINUI_H
#define MAINUI_H

#include <QDialog>
#include <QLayout>
#include <QPushButton>

 #include "piecebutton.h"

class MainUI : public QDialog
{
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;
    PieceButton *aiModeButton;
    PieceButton *personModeButton;
    //QVBoxLayout *black, *white;
    //QGridLayout *game_region;
    //QLabel *text_black, *text_white;
    //PieceImage *pieces[N][N];
    //QPushButton *start, *retract_black, *retract_white;

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
