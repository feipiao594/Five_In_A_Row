#include "mainui.h"
#include "../controller/boardcontroller.h"
#include "../model/manager.h"
#include "selectionui.h"

MainUI::MainUI() : mainLayout(new QHBoxLayout) {
  setWindowTitle("五子棋");
  setFixedSize(QSize(800, 600));

  retract_black = new QPushButton("悔棋");
  retract_white = new QPushButton("悔棋");

  text_black = new QLabel();
  text_white = new QLabel();

  text_black->setText("黑方");
  text_white->setText("白方");

  black = new QVBoxLayout();
  white = new QVBoxLayout();

  black->addWidget(text_black);
  black->addWidget(retract_black);

  white->addWidget(text_white);
  white->addWidget(retract_white);

  game_region = new QGridLayout();
  board = new QWidget;
  setAutoFillBackground(true);
  board->setObjectName("boardWidget");
  board->setStyleSheet("#boardWidget{border-image:url(" +
                       Resource::getInstance()->boardUrl + ");}");
  board->setLayout(game_region);
  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++) {
      pieces[i][j] = new PieceButton(i, j, 28);
      game_region->addWidget(pieces[i][j], i, j);
    }
  setLayout(mainLayout);

  mainLayout->addLayout(black);
  mainLayout->addWidget(board);
  mainLayout->addLayout(white);

  BoardController::getInstance()->setParent(this);

  connect(Manager::getInstance(), &Manager::onDropped,
          BoardController::getInstance(),
          &BoardController::onDroppedController);

  connect(BoardController::getInstance(), &BoardController::updateDropPiece,
          this, &MainUI::showDropPiece);
  connect(BoardController::getInstance(), &BoardController::updateWinner, this,
          &MainUI::onGameOver);

  connect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  connect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);
}

void MainUI::onGameOver(Unit color) {
  QString message;
  disconnect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  disconnect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);
  connect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);
  retract_black->setText("重新开始");
  text_white->hide();
  retract_white->hide();

  switch (color) {
  case Unit::Black:
    message = "黑方胜利！";
    break;
  case Unit::White:
    message = "白方胜利！";
    break;
  case Unit::Empty:
    message = "和局！";
    break;
  }

  text_black->setText(message + "\n游戏已结束，要再来一局吗？");
}

void MainUI::restartGame() {
  retract_black->hide();
  clearPieceColor();
  disconnect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);
  connect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  connect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);
  retract_white->setText("悔棋");
  retract_black->setText("悔棋");
  text_black->setText("黑方");
  text_white->setText("白方");
  text_black->show();
  text_white->show();
  retract_white->show();
  retract_black->show();

  SelectionUI::getInstance()->showSelection();
}

void MainUI::piecePushed(int x, int y) {
  BoardController::getInstance()->pieceClicked(x, y);
}

void MainUI::showDropPiece() {
  auto controller = BoardController::getInstance();
  setPieceColor(controller->getDropPiecePos(), controller->getDropPieceColor());
}
