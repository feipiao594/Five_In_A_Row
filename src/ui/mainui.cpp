#include "mainui.h"
#include "selectionui.h"
#include "../controller/boardcontroller.h"

MainUI::MainUI() : mainLayout(new QHBoxLayout) {
    setWindowTitle("五子棋");
    setFixedSize(QSize(800, 600));
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
    board = new QWidget;
    setAutoFillBackground(true);
    board->setObjectName("boardWidget");
    board->setStyleSheet("#boardWidget{border-image:url("+Resource::getInstance()->boardUrl+");}");
    board->setLayout(game_region);
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            pieces[i][j] = new PieceButton(i, j, 28);
            game_region->addWidget(pieces[i][j], i, j);
        }
    setLayout(mainLayout);
    mainLayout->addLayout(black);
    mainLayout->addWidget(board);
    mainLayout->addLayout(white);
    BoardController::getInstance()->setParent(this);

}
