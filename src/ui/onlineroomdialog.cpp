#include "onlineroomdialog.h"

#include "loadingindicator.h"

#include "../net/authstore.h"
#include "../net/wsclient.h"
#include "../model/resource.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QStringList>
#include <QVBoxLayout>
#include <QClipboard>
#include <QGuiApplication>
#include <QTimer>

static QString displayNameOrDash(const QString& username) {
  return username.isEmpty() ? "—" : username;
}

static QString seatUsernameFromJson(const QJsonValue& v) {
  if (v.isString())
    return v.toString();
  if (!v.isObject())
    return "";
  const auto o = v.toObject();
  const auto u = o.value("username").toString();
  if (!u.isEmpty())
    return u;
  // 兼容其它字段名
  const auto user = o.value("user").toString();
  if (!user.isEmpty())
    return user;
  return o.value("name").toString();
}

static bool seatReadyFromJson(const QJsonValue& v) {
  if (!v.isObject())
    return false;
  const auto o = v.toObject();
  if (o.contains("ready"))
    return o.value("ready").toBool(false);
  return false;
}

OnlineRoomDialog::OnlineRoomDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("联机房间");
  setModal(true);
  setFixedSize(QSize(520, 460));

  auto main = new QVBoxLayout();
  main->setContentsMargins(18, 18, 18, 18);
  main->setSpacing(12);
  setLayout(main);

  title_ = new QLabel("联机房间");
  title_->setAlignment(Qt::AlignHCenter);
  title_->setStyleSheet("font-size: 18px; font-weight: 600;");
  main->addWidget(title_);

  statusBar_ = new QWidget(this);
  auto statusRow = new QHBoxLayout();
  statusRow->setContentsMargins(0, 0, 0, 0);
  statusBar_->setLayout(statusRow);
  status_ = new QLabel();
  status_->setStyleSheet("font-size: 16px; font-weight: 600;");
  statusRow->addWidget(status_, 1);
  main->addWidget(statusBar_);

  roleLabel_ = new QLabel();
  roleLabel_->setStyleSheet("font-size: 16px; font-weight: 600;");
  main->addWidget(roleLabel_);

  connectBtn_ = new QPushButton("连接服务器");
  connectBtn_->setMinimumSize(QSize(140, 40));
  connectBtn_->setStyleSheet("font-size: 16px;");

  connectPage_ = new QWidget(this);
  auto connectPageLayout = new QVBoxLayout();
  connectPageLayout->setContentsMargins(0, 0, 0, 0);
  connectPageLayout->setSpacing(14);
  connectPageLayout->addStretch(1);
  centerSpinner_ = new LoadingIndicator(this, 52);
  connectPageLayout->addWidget(centerSpinner_, 0, Qt::AlignHCenter);
  connectPageLayout->addWidget(connectBtn_, 0, Qt::AlignHCenter);
  connectPageLayout->addStretch(1);
  connectPage_->setLayout(connectPageLayout);

  connectedWidget_ = new QWidget(this);
  auto connectedLayout = new QVBoxLayout();
  connectedLayout->setContentsMargins(0, 0, 0, 0);
  connectedLayout->setSpacing(12);
  connectedWidget_->setLayout(connectedLayout);
  // 页面容器：未连接/连接中/失败 -> connectPage_；已连接 -> connectedWidget_。
  auto pages = new QWidget(this);
  pagesLayout_ = new QStackedLayout();
  pages->setLayout(pagesLayout_);
  pagesLayout_->addWidget(connectPage_);
  pagesLayout_->addWidget(connectedWidget_);
  pagesLayout_->setCurrentWidget(WsClient::getInstance()->isConnected()
                                     ? connectedWidget_
                                     : connectPage_);
  main->addWidget(pages, 1);

  roomLabel_ = new QLabel("房间 ID");
  roomIdEdit_ = new QLineEdit();
  roomIdEdit_->setMinimumHeight(32);
  roomIdEdit_->setStyleSheet("font-size: 14px;");
  createBtn_ = new QPushButton("创建房间");
  joinBtn_ = new QPushButton("加入房间");
  createBtn_->setMinimumSize(QSize(96, 34));
  joinBtn_->setMinimumSize(QSize(96, 34));
  createBtn_->setStyleSheet("font-size: 14px;");
  joinBtn_->setStyleSheet("font-size: 14px;");

  auto roomRow = new QHBoxLayout();
  roomRow->addWidget(roomLabel_);
  roomRow->addWidget(roomIdEdit_, 1);
  roomRow->addWidget(createBtn_);
  roomRow->addWidget(joinBtn_);
  connectedLayout->addLayout(roomRow);

  auto playerPanel = new QWidget(this);
  auto playerLayout = new QVBoxLayout();
  playerLayout->setContentsMargins(12, 0, 12, 0);
  playerPanel->setLayout(playerLayout);
  connectedLayout->addWidget(playerPanel);

  blackLabel_ = new QLabel(this);
  whiteLabel_ = new QLabel(this);
  spectatorsLabel_ = new QLabel("旁观者：—", this);

  blackIcon_ = new QLabel(this);
  whiteIcon_ = new QLabel(this);
  blackIcon_->setFixedSize(QSize(24, 24));
  whiteIcon_->setFixedSize(QSize(24, 24));
  blackIcon_->setPixmap(
      Resource::getInstance()->color2pixmap(Unit::Black, false).scaled(
          18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  whiteIcon_->setPixmap(
      Resource::getInstance()->color2pixmap(Unit::White, false).scaled(
          18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  blackLabel_->setStyleSheet("font-size: 14px;");
  whiteLabel_->setStyleSheet("font-size: 14px;");
  spectatorsLabel_->setStyleSheet("font-size: 14px;");
  spectatorsLabel_->setWordWrap(true);

  auto blackRow = new QHBoxLayout();
  blackRow->addWidget(blackIcon_);
  blackRow->addWidget(blackLabel_, 1);
  playerLayout->addLayout(blackRow);

  auto sep1 = new QFrame(this);
  sep1->setFrameShape(QFrame::HLine);
  sep1->setFrameShadow(QFrame::Sunken);
  sep1->setFixedHeight(1);
  playerLayout->addWidget(sep1);

  auto whiteRow = new QHBoxLayout();
  whiteRow->addWidget(whiteIcon_);
  whiteRow->addWidget(whiteLabel_, 1);
  playerLayout->addLayout(whiteRow);

  auto sep2 = new QFrame(this);
  sep2->setFrameShape(QFrame::HLine);
  sep2->setFrameShadow(QFrame::Sunken);
  sep2->setFixedHeight(1);
  playerLayout->addWidget(sep2);

  playerLayout->addWidget(spectatorsLabel_);

  auto seatRow = new QHBoxLayout();
  seatRow->addStretch(1);
  takeBlackBtn_ = new QPushButton("坐黑");
  takeWhiteBtn_ = new QPushButton("坐白");
  takeSpectatorBtn_ = new QPushButton("观战");
  readyBtn_ = new QPushButton("准备");
  takeBlackBtn_->setMinimumSize(QSize(96, 34));
  takeWhiteBtn_->setMinimumSize(QSize(96, 34));
  takeSpectatorBtn_->setMinimumSize(QSize(96, 34));
  readyBtn_->setMinimumSize(QSize(96, 34));
  takeBlackBtn_->setStyleSheet("font-size: 14px;");
  takeWhiteBtn_->setStyleSheet("font-size: 14px;");
  takeSpectatorBtn_->setStyleSheet("font-size: 14px;");
  readyBtn_->setStyleSheet("font-size: 14px;");
  seatRow->addWidget(takeBlackBtn_);
  seatRow->addWidget(takeWhiteBtn_);
  seatRow->addWidget(takeSpectatorBtn_);
  seatRow->addWidget(readyBtn_);
  connectedLayout->addLayout(seatRow);

  connect(connectBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onConnectClicked);
  connect(createBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onCreateClicked);
  connect(joinBtn_, &QPushButton::clicked, this, &OnlineRoomDialog::onJoinClicked);
  connect(takeBlackBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onTakeBlack);
  connect(takeWhiteBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onTakeWhite);
  connect(takeSpectatorBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onTakeSpectator);
  connect(readyBtn_, &QPushButton::clicked, this,
          &OnlineRoomDialog::onReadyClicked);

  auto ws = WsClient::getInstance();
  connect(ws, &WsClient::connected, this, &OnlineRoomDialog::onWsConnected,
          Qt::UniqueConnection);
  connect(ws, &WsClient::disconnected, this, &OnlineRoomDialog::onWsDisconnected,
          Qt::UniqueConnection);
  connect(ws, &WsClient::errorOccurred, this, &OnlineRoomDialog::onWsError,
          Qt::UniqueConnection);
  connect(ws, &WsClient::eventReceived, this, &OnlineRoomDialog::onWsEvent,
          Qt::UniqueConnection);
  connect(ws, &WsClient::responseReceived, this, &OnlineRoomDialog::onWsResp,
          Qt::UniqueConnection);

  setConnectedUi(ws->isConnected());
  setStatus(ws->isConnected() ? "已连接" : "未连接");
  roleLabel_->setText("身份：未加入");
  blackLabel_->setText("黑方：—");
  whiteLabel_->setText("白方：—");
  refreshButtons();

  connectUiDelayTimer_.setSingleShot(true);
  connect(&connectUiDelayTimer_, &QTimer::timeout, this,
          &OnlineRoomDialog::applyPendingConnectionResult);

  // 初次进入房间页：若未连接则自动尝试连接，先展示至少 1.5s 的加载动画。
  if (!ws->isConnected()) {
    QTimer::singleShot(0, this, [this]() { onConnectClicked(); });
  }
}

void OnlineRoomDialog::setStatus(const QString& s) {
  status_->setText(s);
}

void OnlineRoomDialog::refreshButtons() {
  const bool wsOk = WsClient::getInstance()->isConnected();
  const bool hasRoom = !roomId_.isEmpty();
  createBtn_->setEnabled(wsOk);
  joinBtn_->setEnabled(wsOk);
  const bool blackFree = blackUser_.isEmpty() || myRole_ == "black";
  const bool whiteFree = whiteUser_.isEmpty() || myRole_ == "white";
  takeBlackBtn_->setEnabled(wsOk && hasRoom && blackFree && myRole_ != "black");
  takeWhiteBtn_->setEnabled(wsOk && hasRoom && whiteFree && myRole_ != "white");
  takeSpectatorBtn_->setEnabled(wsOk && hasRoom && myRole_ != "spectator");

  const bool canReady =
      wsOk && hasRoom && (myRole_ == "black" || myRole_ == "white");
  readyBtn_->setEnabled(canReady);

  bool myReady = false;
  if (myRole_ == "black")
    myReady = blackReady_;
  else if (myRole_ == "white")
    myReady = whiteReady_;
  readyBtn_->setText(myReady ? "取消准备" : "准备");
}

void OnlineRoomDialog::updateRoleFromSnapshot(const QJsonObject& snap) {
  const auto seats = snap.value("seats").toObject();
  const auto black = seats.value("black").toObject();
  const auto white = seats.value("white").toObject();
  const auto spectators = snap.value("spectators").toArray();

  const auto username = AuthStore::username();
  myRole_.clear();
  blackUser_ = black.value("username").toString();
  whiteUser_ = white.value("username").toString();

  if (!blackUser_.isEmpty() && blackUser_ == username)
    myRole_ = "black";
  else if (!whiteUser_.isEmpty() && whiteUser_ == username)
    myRole_ = "white";
  else {
    for (const auto& v : spectators) {
      if (v.toString() == username) {
        myRole_ = "spectator";
        break;
      }
    }
  }

  QString roleText = "身份：未加入";
  if (myRole_ == "black")
    roleText = "身份：黑方";
  else if (myRole_ == "white")
    roleText = "身份：白方";
  else if (myRole_ == "spectator")
    roleText = "身份：观战";
  roleLabel_->setText(roleText);
}

void OnlineRoomDialog::setConnectedUi(bool connected) {
  if (pagesLayout_)
    pagesLayout_->setCurrentWidget(connected ? connectedWidget_ : connectPage_);
  title_->setVisible(connected);
  statusBar_->setVisible(connected);
  roleLabel_->setVisible(connected);
  if (connected) {
    centerSpinner_->stop();
    setStatus("已连接");
  }

  if (!connected) {
    roomId_.clear();
    myRole_.clear();
    blackUser_.clear();
    whiteUser_.clear();
    blackReady_ = false;
    whiteReady_ = false;
    roomIdEdit_->setText("");
    roleLabel_->setText("身份：未加入");
    blackLabel_->setText("黑方：—");
    whiteLabel_->setText("白方：—");
    spectatorsLabel_->setText("旁观者：—");
    setConnectUiReconnect();
  }
  refreshButtons();
}

void OnlineRoomDialog::setConnectUiConnecting() {
  if (pagesLayout_)
    pagesLayout_->setCurrentWidget(connectPage_);
  title_->setVisible(false);
  statusBar_->setVisible(false);
  roleLabel_->setVisible(false);

  centerSpinner_->start();
  connectBtn_->setVisible(false);
}

void OnlineRoomDialog::setConnectUiReconnect(const QString& message) {
  Q_UNUSED(message);
  if (pagesLayout_)
    pagesLayout_->setCurrentWidget(connectPage_);
  title_->setVisible(false);
  statusBar_->setVisible(false);
  roleLabel_->setVisible(false);

  centerSpinner_->stop();
  connectBtn_->setVisible(true);
  connectBtn_->setEnabled(true);
  connectBtn_->setText(connectAttemptTimer_.isValid() ? "重连" : "连接服务器");
}

void OnlineRoomDialog::updatePlayersFromSnapshot(const QJsonObject& snap) {
  const auto seats = snap.value("seats").toObject();
  blackUser_ = seatUsernameFromJson(seats.value("black"));
  whiteUser_ = seatUsernameFromJson(seats.value("white"));
}

void OnlineRoomDialog::updateReadyFromSnapshot(const QJsonObject& snap) {
  const auto seats = snap.value("seats").toObject();
  const auto blackV = seats.value("black");
  const auto whiteV = seats.value("white");
  blackReady_ = seatReadyFromJson(blackV);
  whiteReady_ = seatReadyFromJson(whiteV);

  const QString blackName = displayNameOrDash(blackUser_);
  const QString whiteName = displayNameOrDash(whiteUser_);
  blackLabel_->setText(QString("黑方：%1  %2")
                           .arg(blackName, blackReady_ ? "已准备" : "未准备"));
  whiteLabel_->setText(QString("白方：%1  %2")
                           .arg(whiteName, whiteReady_ ? "已准备" : "未准备"));
}

void OnlineRoomDialog::updateSpectatorsFromSnapshot(const QJsonObject& snap) {
  const auto spectators = snap.value("spectators").toArray();
  QStringList names;
  names.reserve(spectators.size());
  for (const auto& v : spectators) {
    QString s;
    if (v.isString())
      s = v.toString();
    else if (v.isObject())
      s = seatUsernameFromJson(v);
    if (!s.isEmpty())
      names.push_back(s);
  }
  spectatorsLabel_->setText(QString("旁观者：%1").arg(
      names.isEmpty() ? "（无）" : names.join("，")));
}

void OnlineRoomDialog::applySnapshot(const QJsonObject& snap) {
  roomId_ = snap.value("roomId").toString();
  if (!roomId_.isEmpty())
    roomIdEdit_->setText(roomId_);

  updatePlayersFromSnapshot(snap);
  updateReadyFromSnapshot(snap);
  updateSpectatorsFromSnapshot(snap);
  updateRoleFromSnapshot(snap);
}

void OnlineRoomDialog::onConnectClicked() {
  if (WsClient::getInstance()->isConnected()) {
    setConnectedUi(true);
    setStatus("已连接");
    return;
  }
  connectingUi_ = true;
  connectAttemptTimer_.restart();
  pendingConnResult_ = PendingConnResult::None;
  pendingConnMessage_.clear();
  connectUiDelayTimer_.stop();
  setConnectUiConnecting();
  WsClient::getInstance()->connectIfNeeded();
}

void OnlineRoomDialog::onCreateClicked() {
  if (!WsClient::getInstance()->isConnected()) {
    setStatus("请先连接服务器");
    return;
  }
  lastReqId_ = WsClient::getInstance()->sendRequest("room.create",
                                                    QJsonObject{{"title", "房间"}});
  setStatus("正在创建房间…");
}

void OnlineRoomDialog::onJoinClicked() {
  if (!WsClient::getInstance()->isConnected()) {
    setStatus("请先连接服务器");
    return;
  }
  const auto id = roomIdEdit_->text().trimmed();
  if (id.isEmpty()) {
    setStatus("请输入房间 ID");
    return;
  }
  lastReqId_ = WsClient::getInstance()->sendRequest("room.join",
                                                   QJsonObject{{"roomId", id}});
  setStatus("正在加入房间…");
}

void OnlineRoomDialog::onTakeBlack() {
  if (roomId_.isEmpty())
    return;
  lastReqId_ = WsClient::getInstance()->sendRequest(
      "room.takeSeat", QJsonObject{{"seat", "black"}});
  setStatus("正在换座…");
}

void OnlineRoomDialog::onTakeWhite() {
  if (roomId_.isEmpty())
    return;
  lastReqId_ = WsClient::getInstance()->sendRequest(
      "room.takeSeat", QJsonObject{{"seat", "white"}});
  setStatus("正在换座…");
}

void OnlineRoomDialog::onTakeSpectator() {
  if (roomId_.isEmpty())
    return;
  lastReqId_ = WsClient::getInstance()->sendRequest(
      "room.takeSeat", QJsonObject{{"seat", "spectator"}});
  setStatus("正在切换为观战…");
}

void OnlineRoomDialog::onReadyClicked() {
  if (roomId_.isEmpty())
    return;
  bool myReady = false;
  if (myRole_ == "black")
    myReady = blackReady_;
  else if (myRole_ == "white")
    myReady = whiteReady_;

  const bool nextReady = !myReady;
  lastReqId_ = WsClient::getInstance()->sendRequest(
      "room.ready", QJsonObject{{"ready", nextReady}});
  setStatus(nextReady ? "已准备，等待开局…" : "已取消准备");
}

void OnlineRoomDialog::onWsConnected() {
  settleConnectionResult(PendingConnResult::Connected);
}

void OnlineRoomDialog::onWsDisconnected() {
  settleConnectionResult(PendingConnResult::Disconnected);
}

void OnlineRoomDialog::onWsError(const QString& message) {
  settleConnectionResult(PendingConnResult::Error, message);
}

void OnlineRoomDialog::settleConnectionResult(PendingConnResult result,
                                              const QString& message) {
  // 只有“正在连接”的状态才需要最低转圈时间；其它断开/报错直接落地 UI。
  if (!connectingUi_) {
    if (result == PendingConnResult::Connected) {
      setConnectedUi(true);
      return;
    }
    setConnectedUi(false);
    setConnectUiReconnect(message);
    return;
  }

  static constexpr int kMinSpinMs = 600;
  const int elapsed = static_cast<int>(connectAttemptTimer_.elapsed());
  const int remain = kMinSpinMs - elapsed;

  pendingConnResult_ = result;
  pendingConnMessage_ = message;

  if (remain > 0) {
    if (!connectUiDelayTimer_.isActive())
      connectUiDelayTimer_.start(remain);
    return;
  }
  applyPendingConnectionResult();
}

void OnlineRoomDialog::applyPendingConnectionResult() {
  if (pendingConnResult_ == PendingConnResult::None)
    return;

  const auto result = pendingConnResult_;
  const auto message = pendingConnMessage_;
  pendingConnResult_ = PendingConnResult::None;
  pendingConnMessage_.clear();

  connectingUi_ = false;
  centerSpinner_->stop();

  if (result == PendingConnResult::Connected) {
    setConnectedUi(true);
    return;
  }

  setConnectedUi(false);
  setConnectUiReconnect(message);
}

void OnlineRoomDialog::onWsEvent(const QString& type, const QJsonObject& payload) {
  if (type == "room.snapshot") {
    applySnapshot(payload);
    refreshButtons();
    return;
  }
  if (type == "match.start") {
    setStatus("已开局");
    accept();
    return;
  }
}

void OnlineRoomDialog::onWsResp(const QString& type, const QString& reqId, bool ok,
                               const QJsonObject& payload,
                               const QString& errorMessage,
                               const QString& errorCode) {
  if (!ok) {
    QString msg = errorMessage.isEmpty() ? "请求失败" : errorMessage;
    if (errorCode == "room_not_found")
      msg = "房间不存在（可能房主已退出/掉线，或房间 ID 复制错误）";
    if (!errorCode.isEmpty())
      msg = QString("%1 [%2]").arg(msg, errorCode);
    setStatus(msg);
    return;
  }
  if (type == "room.create.resp" && reqId == lastReqId_) {
    const auto roomId = payload.value("roomId").toString();
    if (!roomId.isEmpty()) {
      roomId_ = roomId;
      roomIdEdit_->setText(roomId_);
      setStatus("房间已创建");
      refreshButtons();
      if (auto cb = QGuiApplication::clipboard())
        cb->setText(roomId_);
    }
  }
  if (type == "room.join.resp" && reqId == lastReqId_) {
    const auto room = payload.value("room").toObject();
    const auto roomId = room.value("roomId").toString();
    if (!roomId.isEmpty()) {
      applySnapshot(room);
      refreshButtons();
    }
    setStatus("已加入房间");
  }
  if ((type == "room.takeSeat.resp" || type == "room.ready.resp") &&
      reqId == lastReqId_) {
    const auto room = payload.value("room").toObject();
    if (!room.isEmpty()) {
      applySnapshot(room);
      refreshButtons();
    }
    if (type == "room.takeSeat.resp")
      setStatus("换座成功");
    else {
      bool myReady = false;
      if (myRole_ == "black")
        myReady = blackReady_;
      else if (myRole_ == "white")
        myReady = whiteReady_;
      setStatus(myReady ? "已准备，等待开局…" : "已取消准备");
    }
  }
}
