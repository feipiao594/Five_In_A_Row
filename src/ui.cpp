#include "ui.h"

#include <QMessageBox>
#include <QMouseEvent>

void PieceImage::change(int _x, int _y, enum OCCUPATION colour)
{
    if (x == _x && y == _y)
        switch (colour)
        {
        case BLACK:
            setText("黑");
            break;
        case WHITE:
            setText("白");
            break;
        case NONE:
            setText("+");
            break;
        }
}

void PieceImage::on_clicked()
{
    emit move(x, y);
}

PieceImage::PieceImage(int _x, int _y)
    : x(_x), y(_y)
{
    connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
    connect(this, SIGNAL(move(int, int)), &board, SLOT(move(int, int)));
    connect(&board, SIGNAL(moved(int, int, OCCUPATION)), this, SLOT(change(int, int, OCCUPATION)));
}

void MainUI::on_start()
{
    hide();

    mainLayout->removeWidget(start);
    start->hide();

    mainLayout->addLayout(black);
    mainLayout->addLayout(game_region);
    mainLayout->addLayout(white);

    show();
}

void MainUI::on_game_over(enum OCCUPATION player)
{
    QString message;

    disconnect(retract_black, SIGNAL(clicked()), &board, SLOT(blackRetract()));
    disconnect(retract_white, SIGNAL(clicked()), &board, SLOT(whiteRetract()));
    connect(retract_black, SIGNAL(clicked()), this, SLOT(restart()));
    retract_black->setText("重新开始");
    text_white->hide();
    retract_white->hide();

    switch (player)
    {
    case BLACK:
        message = "黑方胜利！";
        break;
    case WHITE:
        message = "白方胜利！";
        break;
    }

    text_black->setText(message + "\n游戏已结束，要再来一局吗？");
}

MainUI::MainUI(QWidget *parent)
    : QWidget(parent)
{
    // 窗口设置
    setWindowTitle("五子棋 by Aryeth");
    setFixedSize(800, 600);
    start = new QPushButton("开始游戏");
    mainLayout = new QHBoxLayout();
    mainLayout->addWidget(start);
    retract_black = new QPushButton("悔棋");
    retract_white = new QPushButton("悔棋");
    text_black = new QLabel();
    text_black->setText("黑方");
    text_white = new QLabel();
    text_white->setText("白方");
    black = new QVBoxLayout();
    black->addWidget(text_black);
    black->addWidget(retract_black);
    white = new QVBoxLayout();
    white->addWidget(text_white);
    white->addWidget(retract_white);
    game_region = new QGridLayout();
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            pieces[i][j] = new PieceImage(i, j);
            pieces[i][j]->setText("+");
            game_region->addWidget(pieces[i][j], i, j);
        }
    setLayout(mainLayout);

    // 信号槽设置
    connect(start, SIGNAL(clicked()), this, SLOT(on_start()));
    connect(&board, SIGNAL(win(OCCUPATION)), this, SLOT(on_game_over(OCCUPATION)));
    connect(retract_black, SIGNAL(clicked()), &board, SLOT(blackRetract()));
    connect(retract_white, SIGNAL(clicked()), &board, SLOT(whiteRetract()));
    connect(&board, SIGNAL(error(OCCUPATION)), this, SLOT(on_error(OCCUPATION)));
}

void MainUI::restart()
{
    retract_black->hide();
    board.clear();
    disconnect(retract_black, SIGNAL(clicked()), this, SLOT(restart()));
    connect(retract_black, SIGNAL(clicked()), &board, SLOT(blackRetract()));
    connect(retract_white, SIGNAL(clicked()), &board, SLOT(whiteRetract()));
    retract_white->setText("悔棋");
    retract_black->setText("悔棋");
    text_black->setText("黑方");
    text_white->setText("白方");
    text_black->show();
    text_white->show();
    retract_white->show();
    retract_black->show();
}

void MainUI::on_error(enum OCCUPATION player)
{
    switch (player)
    {
    case BLACK:
        text_black->setText("黑方\n非法操作！");
        break;
    case WHITE:
        text_white->setText("白方\n非法操作！");
        break;
    case NONE:
        return;
    }
    connect(&board, SIGNAL(moved(int, int, OCCUPATION)), this, SLOT(refresh()));
}

void MainUI::refresh()
{
    disconnect(&board, SIGNAL(moved(int, int, OCCUPATION)), this, SLOT(refresh()));
    text_black->setText("黑方");
    text_white->setText("白方");
}
