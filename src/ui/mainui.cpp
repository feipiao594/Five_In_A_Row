#include "mainui.h"
#include "../controller/boardcontroller.h"
#include "../controller/selectioncontroller.h"
#include "../model/manager.h"
#include "selectionui.h"

MainUI::MainUI() : mainLayout(new QHBoxLayout) {
  setWindowTitle("五子棋");
  setFixedSize(QSize(800, 600));

  retract_black = new QPushButton("悔棋");
  retract_white = new QPushButton("悔棋");

  img_black = new QLabel();
  img_white = new QLabel();

  text_black = new QLabel();
  text_white = new QLabel();

  img_black->setAlignment(Qt::AlignCenter);
  img_white->setAlignment(Qt::AlignCenter);

  text_black->setAlignment(Qt::AlignCenter);
  text_white->setAlignment(Qt::AlignCenter);

  text_black->setFont(QFont("Arial", 14));
  text_white->setFont(QFont("Arial", 14));

  auto blackImg = Resource::getInstance()->color2pixmap(Unit::Black, false);
  blackImg = blackImg.scaled(60, 60, Qt::KeepAspectRatio);
  img_black->setPixmap(blackImg);

  auto whiteImg = Resource::getInstance()->color2pixmap(Unit::White, false);
  whiteImg = whiteImg.scaled(60, 60, Qt::KeepAspectRatio);
  img_white->setPixmap(whiteImg);

  text_black->setText("黑方");
  text_white->setText("白方");

  black = new QVBoxLayout();
  white = new QVBoxLayout();

  black->addWidget(text_black);
  black->addWidget(img_black);
  black->addWidget(retract_black);

  white->addWidget(text_white);
  white->addWidget(img_white);
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
  connect(Manager::getInstance(), &Manager::onGameOver,
          BoardController::getInstance(),
          &BoardController::onGameOverController);
  connect(Manager::getInstance(), &Manager::onUndoDone,
          BoardController::getInstance(),
          &BoardController::onUndoDoneController);

  connect(BoardController::getInstance(), &BoardController::updateUndo, this,
          &MainUI::clearPiecePos);
  connect(BoardController::getInstance(), &BoardController::updateDropPiece,
          this, &MainUI::showDropPiece);
  connect(BoardController::getInstance(), &BoardController::updateWinner, this,
          &MainUI::onGameOver);

  connect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  connect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);

  connect(this, &MainUI::onBlackRetract, Manager::getInstance(),
          &Manager::blackUndo);
  connect(this, &MainUI::onWhiteRetract, Manager::getInstance(),
          &Manager::whiteUndo);
}

void MainUI::onGameOver(Unit color) {
  QString message;
  disconnect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  disconnect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);
  connect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);
  retract_black->setText("重新开始");
  text_white->hide();
  img_white->hide();
  retract_white->hide();
  retract_white->setEnabled(true);
  retract_black->setEnabled(true);

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

  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++) {
      pieces[i][j]->stopUsing();
    }

  text_black->setText(message + "\n游戏已结束，要再来一局吗？");
  img_black->setPixmap(Resource::getInstance()->color2pixmap(color, false));
}

void MainUI::clearPiecePos(int x, int y) { pieces[x][y]->clearColor(); }

void MainUI::restartGame() {
  retract_black->hide();
  clearPieceColor();

  disconnect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);

  connect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
  connect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);

  retract_white->setText("悔棋");
  retract_black->setText("悔棋");

  auto blackImg = Resource::getInstance()->color2pixmap(Unit::Black, false);
  blackImg = blackImg.scaled(60, 60, Qt::KeepAspectRatio);

  auto whiteImg = Resource::getInstance()->color2pixmap(Unit::White, false);
  whiteImg = whiteImg.scaled(60, 60, Qt::KeepAspectRatio);

  img_black->setPixmap(blackImg);
  img_white->setPixmap(whiteImg);

  text_black->setText("黑方");
  text_white->setText("白方");

  retract_white->setEnabled(true);
  retract_black->setEnabled(true);

  img_white->show();
  text_black->show();
  text_white->show();

  retract_white->show();
  retract_black->show();
  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++) {
      pieces[i][j]->clearColor();
    }
  Manager::getInstance()->restart();
  BoardController::getInstance()->restartBoardController();
  SelectionUI::getInstance()->showSelection();
}

void MainUI::changeAiModeView(Unit color) {
  if (color == Unit::Black) {
    retract_white->setEnabled(false);
    text_white->setText("白方(电脑)");
  } else {
    retract_black->setEnabled(false);
    text_black->setText("黑方(电脑)");
  }
}

void MainUI::piecePushed(int x, int y) {
  BoardController::getInstance()->pieceClicked(x, y);
}

void MainUI::showDropPiece() {
  auto controller = BoardController::getInstance();
  setPieceColor(controller->getDropPiecePos(), controller->getDropPieceColor());
}
