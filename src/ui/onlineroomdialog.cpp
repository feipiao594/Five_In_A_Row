#include "onlineroomdialog.h"

#include "../net/authstore.h"
#include "../net/wsclient.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QClipboard>
#include <QGuiApplication>

OnlineRoomDialog::OnlineRoomDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("联机房间");
  setModal(true);
  setFixedSize(QSize(520, 520));

  auto main = new QVBoxLayout();
  main->setContentsMargins(18, 18, 18, 18);
  main->setSpacing(12);
  setLayout(main);

  title_ = new QLabel("联机房间");
  title_->setAlignment(Qt::AlignHCenter);
  title_->setStyleSheet("font-size: 18px; font-weight: 600;");
  main->addWidget(title_);

  status_ = new QLabel();
  status_->setStyleSheet("font-size: 12px; color: #666;");
  main->addWidget(status_);

  roleLabel_ = new QLabel();
  roleLabel_->setStyleSheet("font-size: 12px; color: #666;");
  main->addWidget(roleLabel_);

  connectBtn_ = new QPushButton("连接服务器");
  connectBtn_->setMinimumSize(QSize(120, 34));
  connectBtn_->setStyleSheet("font-size: 14px;");
  main->addWidget(connectBtn_, 0, Qt::AlignRight);

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
  main->addLayout(roomRow);

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
  main->addLayout(seatRow);

  snapshotView_ = new QTextEdit();
  snapshotView_->setReadOnly(true);
  snapshotView_->setStyleSheet("font-size: 12px;");
  main->addWidget(snapshotView_, 1);

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
  connect(ws, &WsClient::errorOccurred, this, &OnlineRoomDialog::onWsError,
          Qt::UniqueConnection);
  connect(ws, &WsClient::eventReceived, this, &OnlineRoomDialog::onWsEvent,
          Qt::UniqueConnection);
  connect(ws, &WsClient::responseReceived, this, &OnlineRoomDialog::onWsResp,
          Qt::UniqueConnection);

  setStatus(ws->isConnected() ? "已连接" : "未连接");
  roleLabel_->setText("身份：未加入");
  refreshButtons();
}

void OnlineRoomDialog::setStatus(const QString& s) {
  status_->setText(s);
}

void OnlineRoomDialog::refreshButtons() {
  const bool hasRoom = !roomId_.isEmpty();
  createBtn_->setEnabled(true);
  joinBtn_->setEnabled(true);
  const bool blackFree = blackUser_.isEmpty() || myRole_ == "black";
  const bool whiteFree = whiteUser_.isEmpty() || myRole_ == "white";
  takeBlackBtn_->setEnabled(hasRoom && blackFree && myRole_ != "black");
  takeWhiteBtn_->setEnabled(hasRoom && whiteFree && myRole_ != "white");
  takeSpectatorBtn_->setEnabled(hasRoom && myRole_ != "spectator");
  readyBtn_->setEnabled(hasRoom && (myRole_ == "black" || myRole_ == "white"));
}

void OnlineRoomDialog::renderSnapshot(const QJsonObject& snap) {
  snapshotView_->setPlainText(
      QString::fromUtf8(QJsonDocument(snap).toJson(QJsonDocument::Indented)));
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

void OnlineRoomDialog::onConnectClicked() {
  setStatus("正在连接…");
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
  lastReqId_ =
      WsClient::getInstance()->sendRequest("room.ready", QJsonObject{{"ready", true}});
  setStatus("已准备，等待开局…");
}

void OnlineRoomDialog::onWsConnected() {
  setStatus("已连接");
}

void OnlineRoomDialog::onWsError(const QString& message) {
  setStatus(QString("连接失败：%1").arg(message));
}

void OnlineRoomDialog::onWsEvent(const QString& type, const QJsonObject& payload) {
  if (type == "room.snapshot") {
    roomId_ = payload.value("roomId").toString();
    if (!roomId_.isEmpty())
      roomIdEdit_->setText(roomId_);
    updateRoleFromSnapshot(payload);
    renderSnapshot(payload);
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
      roomId_ = roomId;
      roomIdEdit_->setText(roomId_);
      updateRoleFromSnapshot(room);
      renderSnapshot(room);
      refreshButtons();
    }
    setStatus("已加入房间");
  }
  if ((type == "room.takeSeat.resp" || type == "room.ready.resp") &&
      reqId == lastReqId_) {
    const auto room = payload.value("room").toObject();
    if (!room.isEmpty()) {
      const auto roomId = room.value("roomId").toString();
      if (!roomId.isEmpty()) {
        roomId_ = roomId;
        roomIdEdit_->setText(roomId_);
      }
      updateRoleFromSnapshot(room);
      renderSnapshot(room);
      refreshButtons();
    }
    if (type == "room.takeSeat.resp")
      setStatus("换座成功");
    else
      setStatus("已准备，等待开局…");
  }
}
