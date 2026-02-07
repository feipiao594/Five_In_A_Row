#include "mainui.h"
#include "menuui.h"

#include <QtMath>

#include "../controller/boardcontroller.h"
#include "../model/manager.h"
#include "../net/onlinesession.h"
#include "../net/wsclient.h"
#include "onlineroomdialog.h"

MainUI::MainUI() : mainLayout(new QHBoxLayout) {
    setWindowTitle("五子棋");
    setFixedSize(QSize(900, 600));

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

    board = new QWidget(this);
    board->setObjectName("boardWidget");
    board->setStyleSheet("#boardWidget{border-image:url(" + Resource::getInstance()->boardUrl +
                         ");}");
    board->setFixedSize(QSize(596, 604));

    grid_container = new QWidget(board);
    grid_container->setStyleSheet("background: transparent;");
    game_region = new QGridLayout(grid_container);

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            // BoardController/HumanPlayer 以 (col, row) 传递点击坐标。
            pieces[r][c] = new PieceButton(c, r, 28, grid_container);
            game_region->addWidget(pieces[r][c], r, c, Qt::AlignCenter);
        }
    }
    configureBoardGrid();
    setLayout(mainLayout);

    mainLayout->addLayout(black);
    mainLayout->addWidget(board);
    mainLayout->addLayout(white);

    BoardController::getInstance()->setParent(this);

    connect(Manager::getInstance(), &Manager::onDropped, BoardController::getInstance(),
            &BoardController::onDroppedController);
    connect(Manager::getInstance(), &Manager::onGameOver, BoardController::getInstance(),
            &BoardController::onGameOverController);
    connect(Manager::getInstance(), &Manager::onUndoDone, BoardController::getInstance(),
            &BoardController::onUndoDoneController);

    connect(BoardController::getInstance(), &BoardController::updateUndo, this,
            &MainUI::clearPiecePos);
    connect(BoardController::getInstance(), &BoardController::updateDropPiece, this,
            &MainUI::showDropPiece);
    connect(BoardController::getInstance(), &BoardController::updateWinner, this,
            &MainUI::onGameOver);

    connect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
    connect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);

    connect(this, &MainUI::onBlackRetract, Manager::getInstance(), &Manager::blackUndo);
    connect(this, &MainUI::onWhiteRetract, Manager::getInstance(), &Manager::whiteUndo);
}

void MainUI::prepareNewGame() {
    clearPieceColor();
    configureBoardGrid();

    // 还原按钮连接与文案（避免上一局 GameOver 把按钮改成“重新开始”）。
    disconnect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);
    disconnect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
    disconnect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);

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

    text_black->setText(blackSideText);
    text_white->setText(whiteSideText);

    img_white->show();
    text_black->show();
    text_white->show();
    retract_white->show();
    retract_black->show();

    retract_black->setEnabled(blackCanUndo);
    retract_white->setEnabled(whiteCanUndo);
}

void MainUI::onGameOver(Unit color) {
    QString message;
    disconnect(retract_black, &QPushButton::clicked, this, &MainUI::blackRetract);
    disconnect(retract_white, &QPushButton::clicked, this, &MainUI::whiteRetract);
    connect(retract_black, &QPushButton::clicked, this, &MainUI::restartGame);
    const bool isOnline = OnlineSession::getInstance()->isActive();
    retract_black->setText(isOnline ? "返回房间" : "重新开始");
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

    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            pieces[r][c]->stopUsing();

    text_black->setText(isOnline ? (message + "\n游戏已结束，返回房间？")
                                 : (message + "\n游戏已结束，要再来一局吗？"));
    img_black->setPixmap(Resource::getInstance()->color2pixmap(color, false));
}

void MainUI::clearPiecePos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return;
    pieces[y][x]->clearColor();
}

void MainUI::setPieceColor(std::pair<int, int> pos, Unit color) {
    auto [x, y] = pos;
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return;
    pieces[y][x]->setColor(color);
}

void MainUI::restartGame() {
    prepareNewGame();
    if (OnlineSession::getInstance()->isActive()) {
        hide();

        OnlineRoomDialog dialog(this);
        dialog.setModal(true);

        // 若连接已断开，让用户手动点“连接服务器”；这里不自动 connectIfNeeded，避免误触。
        if (dialog.exec() == QDialog::Accepted) {
            return;
        }

        // 用户关闭房间页：退出联机模式，回到菜单。
        OnlineSession::getInstance()->stop();
        WsClient::getInstance()->disconnectNow();
        BoardController::getInstance()->setUseManagerTurn(true);
        BoardController::getInstance()->restartBoardController();
        MenuUI::getInstance()->showMenu();
        return;
    }

    Manager::getInstance()->restart();
    BoardController::getInstance()->setUseManagerTurn(true);
    BoardController::getInstance()->restartBoardController();
    MenuUI::getInstance()->showMenu();
}

void MainUI::setComputerSides(bool blackIsComputer, bool whiteIsComputer) {
    blackSideText = blackIsComputer ? "黑方(电脑)" : "黑方";
    whiteSideText = whiteIsComputer ? "白方(电脑)" : "白方";
    blackCanUndo = !blackIsComputer;
    whiteCanUndo = !whiteIsComputer;

    text_black->setText(blackSideText);
    text_white->setText(whiteSideText);
    retract_black->setEnabled(blackCanUndo);
    retract_white->setEnabled(whiteCanUndo);
}

void MainUI::setUndoEnabled(bool blackEnabled, bool whiteEnabled) {
    blackCanUndo = blackEnabled;
    whiteCanUndo = whiteEnabled;
    retract_black->setEnabled(blackCanUndo);
    retract_white->setEnabled(whiteCanUndo);
}

void MainUI::piecePushed(int x, int y) {
    BoardController::getInstance()->pieceClicked(x, y);
}

void MainUI::showDropPiece() {
    auto controller = BoardController::getInstance();
    setPieceColor(controller->getDropPiecePos(), controller->getDropPieceColor());
}

void MainUI::configureBoardGrid() {
    // board.png：745x755
    static constexpr double kBaseW = 745.0;
    static constexpr double kBaseH = 755.0;
    // 用户测得的交叉点中心：第一点约 33.5px，步长约 48px（线宽 2px）。
    static constexpr double kFirst = 30.875;
    static constexpr double kStep = 48.125;

    const int boardW = board->width();
    const int boardH = board->height();
    const double sx = boardW / kBaseW;
    const double sy = boardH / kBaseH;

    const int btn = pieces[0][0]->width();

    const double firstX = kFirst * sx;
    const double firstY = kFirst * sy;
    const double stepX = kStep * sx;
    const double stepY = kStep * sy;

    int spacingX = qMax(0, qRound(stepX - btn));
    int spacingY = qMax(0, qRound(stepY - btn));

    int left = qMax(0, qRound(firstX - btn / 2.0));
    int top = qMax(0, qRound(firstY - btn / 2.0));

    const int gridW = btn + (BOARD_SIZE - 1) * (btn + spacingX);
    const int gridH = btn + (BOARD_SIZE - 1) * (btn + spacingY);

    if (left + gridW > boardW)
        left = qMax(0, boardW - gridW);
    if (top + gridH > boardH)
        top = qMax(0, boardH - gridH);

    grid_container->setGeometry(left, top, gridW, gridH);
    game_region->setContentsMargins(0, 0, 0, 0);
    game_region->setHorizontalSpacing(spacingX);
    game_region->setVerticalSpacing(spacingY);
}
