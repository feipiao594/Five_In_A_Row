#ifndef MAINUI_H
#define MAINUI_H

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

#include "board.h"

class PieceImage : public QPushButton
{
    Q_OBJECT

private:
    int x, y;

public:
    PieceImage(int i, int j);

public slots:
    void on_clicked();
    void change(int, int, enum OCCUPATION);

signals:
    void move(int, int);
};

class MainUI : public QWidget
{
    Q_OBJECT

private:
    QHBoxLayout *mainLayout;
    QVBoxLayout *black, *white;
    QGridLayout *game_region;
    QLabel *text_black, *text_white;
    PieceImage *pieces[N][N];
    QPushButton *start, *retract_black, *retract_white;

public slots:
    void on_start();
    void on_game_over(enum OCCUPATION);
    void on_error(enum OCCUPATION);
    void restart();
    void refresh();

public:
    MainUI(QWidget *parent = nullptr);
};
#endif // MAINUI_H
