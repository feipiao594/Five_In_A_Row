#include "onlinesession.h"

#include "authstore.h"
#include "wsclient.h"

#include "../controller/boardcontroller.h"
#include "../ui/mainui.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

static Unit strToUnit(const QString& s) {
  if (s == "black")
    return Unit::Black;
  if (s == "white")
    return Unit::White;
  return Unit::Empty;
}

static QString unitToStr(Unit u) {
  if (u == Unit::Black)
    return "black";
  if (u == Unit::White)
    return "white";
  return "";
}

OnlineSession::OnlineSession(QObject* parent) : QObject(parent) {}

OnlineSession* OnlineSession::getInstance() {
  static OnlineSession* singleton = nullptr;
  if (!singleton)
    singleton = new OnlineSession();
  return singleton;
}

void OnlineSession::bindSignals() {
  auto ws = WsClient::getInstance();
  connect(ws, &WsClient::eventReceived, this, &OnlineSession::onWsEvent,
          Qt::UniqueConnection);
  connect(ws, &WsClient::responseReceived, this, &OnlineSession::onWsResp,
          Qt::UniqueConnection);

  connect(BoardController::getInstance(), &BoardController::boardClicked, this,
          &OnlineSession::onBoardClicked, Qt::UniqueConnection);
}

void OnlineSession::unbindSignals() {
  QObject::disconnect(WsClient::getInstance(), nullptr, this, nullptr);
  QObject::disconnect(BoardController::getInstance(), nullptr, this, nullptr);
}

void OnlineSession::start() {
  if (active_)
    return;
  active_ = true;
  localColor_ = Unit::Empty;
  roomId_.clear();
  matchId_.clear();
  turn_ = Unit::Black;

  auto board = BoardController::getInstance();
  board->setUseManagerTurn(false);
  board->setIsYourTurn(false);
  board->setWhoTurn(Unit::Black);

  bindSignals();
  emit stateChanged();
}

void OnlineSession::stop() {
  if (!active_)
    return;
  active_ = false;
  localColor_ = Unit::Empty;
  roomId_.clear();
  matchId_.clear();
  turn_ = Unit::Black;

  unbindSignals();
  BoardController::getInstance()->setUseManagerTurn(true);
  emit stateChanged();
}

void OnlineSession::onWsEvent(const QString& type, const QJsonObject& payload) {
  if (!active_)
    return;
  if (type == "auth.kicked") {
    QMessageBox::information(nullptr, "提示", "账号已在其他设备登录");
    AuthStore::clearLogin();
    stop();
    return;
  }
  if (type == "room.snapshot")
    return handleRoomSnapshot(payload);
  if (type == "match.start")
    return handleMatchStart(payload);
  if (type == "match.moved")
    return handleMatchMoved(payload);
  if (type == "match.over")
    return handleMatchOver(payload);
}

void OnlineSession::onWsResp(const QString& type, const QString& reqId, bool ok,
                            const QJsonObject& payload,
                            const QString& errorMessage,
                            const QString& errorCode) {
  Q_UNUSED(reqId);
  Q_UNUSED(payload);
  Q_UNUSED(errorCode);
  if (!active_)
    return;
  if (type != "match.move.resp")
    return;
  if (ok)
    return;

  // 走到这里说明服务端拒绝了本次落子；恢复为“仍然轮到我（若满足）”。
  const bool myTurn = (localColor_ != Unit::Empty && localColor_ == turn_);
  BoardController::getInstance()->setIsYourTurn(myTurn);
  BoardController::getInstance()->setWhoTurn(turn_);

  QMessageBox::information(nullptr, "提示",
                           errorMessage.isEmpty() ? "落子失败" : errorMessage);
}

void OnlineSession::handleRoomSnapshot(const QJsonObject& payload) {
  roomId_ = payload.value("roomId").toString();

  const auto seats = payload.value("seats").toObject();
  const auto black = seats.value("black").toObject();
  const auto white = seats.value("white").toObject();
  const auto username = AuthStore::username();

  if (!black.isEmpty() && black.value("username").toString() == username)
    localColor_ = Unit::Black;
  else if (!white.isEmpty() && white.value("username").toString() == username)
    localColor_ = Unit::White;
  else
    localColor_ = Unit::Empty;
}

void OnlineSession::handleMatchStart(const QJsonObject& payload) {
  matchId_ = payload.value("matchId").toString();
  turn_ = strToUnit(payload.value("turn").toString());

  auto ui = MainUI::getInstance();
  ui->prepareNewGame();
  ui->show();

  // 联机对局暂不支持本地悔棋按钮。
  ui->setUndoEnabled(false, false);

  BoardController::getInstance()->restartBoardController();
  BoardController::getInstance()->setIsYourTurn(localColor_ != Unit::Empty &&
                                               localColor_ == turn_);
  BoardController::getInstance()->setWhoTurn(turn_);

  // 若有历史 moves（重连/同步），按顺序落子。
  const auto moves = payload.value("moves").toArray();
  for (const auto& mv : moves) {
    const auto o = mv.toObject();
    const auto color = strToUnit(o.value("color").toString());
    const auto coord = o.value("coord").toObject();
    const int row = coord.value("row").toInt(-1);
    const int col = coord.value("col").toInt(-1);
    if (row >= 0 && col >= 0)
      ui->setPieceColor(std::make_pair(col, row), color);
  }
}

void OnlineSession::handleMatchMoved(const QJsonObject& payload) {
  const auto move = payload.value("move").toObject();
  const auto color = strToUnit(move.value("color").toString());
  const auto coord = move.value("coord").toObject();
  const int row = coord.value("row").toInt(-1);
  const int col = coord.value("col").toInt(-1);

  if (row >= 0 && col >= 0)
    MainUI::getInstance()->setPieceColor(std::make_pair(col, row), color);

  turn_ = strToUnit(payload.value("turn").toString());
  BoardController::getInstance()->setWhoTurn(turn_);
  BoardController::getInstance()->setIsYourTurn(localColor_ != Unit::Empty &&
                                               localColor_ == turn_);
}

void OnlineSession::handleMatchOver(const QJsonObject& payload) {
  const auto winner = payload.value("winner").toString();
  Unit w = Unit::Empty;
  if (winner == "black")
    w = Unit::Black;
  else if (winner == "white")
    w = Unit::White;
  MainUI::getInstance()->onGameOver(w);
}

void OnlineSession::onBoardClicked(int x, int y) {
  if (!active_)
    return;
  if (localColor_ == Unit::Empty)
    return;
  if (turn_ != localColor_)
    return;

  // 防止快速多次点击：等待服务端的 match.moved（或 resp 失败）再恢复可点。
  BoardController::getInstance()->setIsYourTurn(false);

  auto ws = WsClient::getInstance();
  QJsonObject payload;
  payload["matchId"] = matchId_;
  QJsonObject coord;
  coord["row"] = y;
  coord["col"] = x;
  payload["coord"] = coord;

  ws->sendRequest("match.move", payload);
}
