#include "selectioncontroller.h"
#include "../model/manager.h"
#include "boardcontroller.h"

void SelectionController::startLocal(bool blackIsComputer, bool whiteIsComputer) {
    auto mgr = Manager::getInstance();
    auto boardController = BoardController::getInstance();

    // 清理上一次开局遗留的输入连接（只会有 HumanPlayer 监听这个信号）。
    QObject::disconnect(boardController, &BoardController::boardClicked, nullptr, nullptr);

    auto black = blackIsComputer ? static_cast<Player*>(new AiPlayer(Unit::Black, mgr, mgr))
                                 : static_cast<Player*>(new HumanPlayer(Unit::Black, mgr, mgr));
    auto white = whiteIsComputer ? static_cast<Player*>(new AiPlayer(Unit::White, mgr, mgr))
                                 : static_cast<Player*>(new HumanPlayer(Unit::White, mgr, mgr));

    mgr->setPlayers(black, white);

    if (auto human = qobject_cast<HumanPlayer*>(black))
        connect(boardController, &BoardController::boardClicked, human, &HumanPlayer::onBoardClick,
                Qt::UniqueConnection);
    if (auto human = qobject_cast<HumanPlayer*>(white))
        connect(boardController, &BoardController::boardClicked, human, &HumanPlayer::onBoardClick,
                Qt::UniqueConnection);

    emit updateComputerSides(blackIsComputer, whiteIsComputer);
    boardController->restartBoardController();
    mgr->restart();
    boardController->updateIsYourTurn();
}

void SelectionController::startOnline() {
    // 暂未实现：未来可在这里创建 NetworkPlayer，并接入网络模块。
}
