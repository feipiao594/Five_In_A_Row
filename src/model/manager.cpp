#include "manager.h"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

static QSettings makeSettings() {
    const auto baseDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(baseDir);
    return QSettings(baseDir + "/settings.ini", QSettings::IniFormat);
}

Unit Manager::getCurColor() const {
    return match->curColor();
}

Unit Manager::getWinner() const {
    return match->winner();
}

Coordinate Manager::getLatestCoord() const {
    return match->latestCoord();
}

QVector<Coordinate> Manager::getUndoList() const {
    return match->undoList();
}

void Manager::setAiDelayMs(int delayMs) {
    aiDelayMs = std::max(0, delayMs);
    auto settings = makeSettings();
    settings.setValue("ai/aiDelayMs", aiDelayMs);
}

Manager::Manager() {
    match = new Match(this);

    auto settings = makeSettings();
    aiDelayMs = settings.value("ai/aiDelayMs", aiDelayMs).toInt();

    connect(match, &Match::onDropped, this, &Manager::onMatchDropped);
    connect(match, &Match::onGameOver, this, &Manager::onMatchEnded);
    connect(match, &Match::onUndoDone, this, &Manager::onMatchUndoDone);
    connect(match, &Match::onOverlap, this, &Manager::onMatchOverlap);
}

void Manager::drop(Coordinate coord) {
    match->drop(coord);
}

void Manager::blackUndo() {
    if (!blackPlayer || !blackPlayer->requiresLocalInput())
        return;
    match->blackUndo();
}

void Manager::whiteUndo() {
    if (!whitePlayer || !whitePlayer->requiresLocalInput())
        return;
    match->whiteUndo();
}

void Manager::restart() {
    match->restart();
    if (blackPlayer)
        blackPlayer->reset();
    if (whitePlayer)
        whitePlayer->reset();
    syncPlayersToMatch();
    requestTurn();
}

void Manager::setPlayers(Player* black, Player* white) {
    if (blackPlayer)
        blackPlayer->deleteLater();
    if (whitePlayer)
        whitePlayer->deleteLater();

    blackPlayer = black;
    whitePlayer = white;

    if (blackPlayer && blackPlayer->parent() != this)
        blackPlayer->setParent(this);
    if (whitePlayer && whitePlayer->parent() != this)
        whitePlayer->setParent(this);

    if (blackPlayer)
        connect(blackPlayer, &Player::moveReady, this, &Manager::onPlayerMove,
                Qt::QueuedConnection);
    if (whitePlayer)
        connect(whitePlayer, &Player::moveReady, this, &Manager::onPlayerMove,
                Qt::QueuedConnection);

    syncPlayersToMatch();
    requestTurn();
}

bool Manager::isLocalTurn() const {
    auto player = currentPlayer();
    return player ? player->requiresLocalInput() : true;
}

Player* Manager::currentPlayer() const {
    return (match->curColor() == Unit::Black) ? blackPlayer : whitePlayer;
}

void Manager::requestTurn() {
    if (match->isOver())
        return;
    auto player = currentPlayer();
    if (!player)
        return;
    player->requestTurn();
}

void Manager::onPlayerMove(Coordinate coord) {
    if (match->isOver())
        return;
    if (coord.row < 0 || coord.col < 0)
        return;

    auto player = qobject_cast<Player*>(sender());
    if (!player)
        return;
    if (player != currentPlayer())
        return;
    if (player->unit() != match->curColor())
        return;

    match->drop(coord);
}

void Manager::onMatchDropped() {
    Coordinate coord = match->latestCoord();
    Unit unit = match->boardView().getUnit(coord);
    notifyMoveApplied(coord, unit);
    emit onDropped();
    requestTurn();
}

void Manager::onMatchEnded() {
    emit onGameOver();
}

void Manager::onMatchUndoDone() {
    for (auto coord : match->undoList())
        notifyMoveReverted(coord);
    emit onUndoDone();
    requestTurn();
}

void Manager::onMatchOverlap() {
    // 非法落子（重叠）不应结束回合：继续请求当前玩家重新输入。
    emit onOverlap();
    requestTurn();
}

void Manager::syncPlayersToMatch() {
    if (blackPlayer)
        blackPlayer->onCoreReset();
    if (whitePlayer)
        whitePlayer->onCoreReset();

    const auto& b = match->boardView();
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            Coordinate coord(r, c);
            Unit unit = b.getUnit(coord);
            if (unit != Unit::Empty)
                notifyMoveApplied(coord, unit);
        }
    }
}

void Manager::notifyMoveApplied(Coordinate coord, Unit unit) {
    if (blackPlayer)
        blackPlayer->onCoreMoveApplied(coord, unit);
    if (whitePlayer)
        whitePlayer->onCoreMoveApplied(coord, unit);
}

void Manager::notifyMoveReverted(Coordinate coord) {
    if (blackPlayer)
        blackPlayer->onCoreMoveReverted(coord);
    if (whitePlayer)
        whitePlayer->onCoreMoveReverted(coord);
}
