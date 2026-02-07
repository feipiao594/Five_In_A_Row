#ifndef ONLINEROOMDIALOG_H
#define ONLINEROOMDIALOG_H

#include <QDialog>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QTimer>

class QLabel;
class QLineEdit;
class QPushButton;
class QStackedLayout;

class OnlineRoomDialog : public QDialog {
  Q_OBJECT

public:
  explicit OnlineRoomDialog(QWidget* parent = nullptr);

  QString roomId() const { return roomId_; }

private slots:
  void onConnectClicked();
  void onCreateClicked();
  void onJoinClicked();
  void onTakeBlack();
  void onTakeWhite();
  void onTakeSpectator();
  void onReadyClicked();

  void onWsConnected();
  void onWsDisconnected();
  void onWsError(const QString& message);
  void onWsEvent(const QString& type, const QJsonObject& payload);
  void onWsResp(const QString& type, const QString& reqId, bool ok,
                const QJsonObject& payload, const QString& errorMessage,
                const QString& errorCode);

private:
  enum class PendingConnResult { None, Connected, Disconnected, Error };

  void setStatus(const QString& s);
  void refreshButtons();
  void setConnectedUi(bool connected);
  void setConnectUiConnecting();
  void setConnectUiReconnect(const QString& message = QString());
  void settleConnectionResult(PendingConnResult result,
                              const QString& message = QString());
  void applyPendingConnectionResult();
  void applySnapshot(const QJsonObject& snap);
  void updateRoleFromSnapshot(const QJsonObject& snap);
  void updatePlayersFromSnapshot(const QJsonObject& snap);
  void updateReadyFromSnapshot(const QJsonObject& snap);
  void updateSpectatorsFromSnapshot(const QJsonObject& snap);

  QLabel* title_;
  QWidget* statusBar_ = nullptr;
  QLabel* status_;
  QLabel* roleLabel_;
  QPushButton* connectBtn_;
  class LoadingIndicator* centerSpinner_ = nullptr;
  bool connectingUi_ = false;
  QElapsedTimer connectAttemptTimer_;
  QTimer connectUiDelayTimer_;
  PendingConnResult pendingConnResult_ = PendingConnResult::None;
  QString pendingConnMessage_;

  QWidget* connectPage_ = nullptr;
  QStackedLayout* pagesLayout_ = nullptr;
  QWidget* connectedWidget_ = nullptr;

  QLabel* roomLabel_;
  QLineEdit* roomIdEdit_;
  QPushButton* createBtn_;
  QPushButton* joinBtn_;

  QPushButton* takeBlackBtn_;
  QPushButton* takeWhiteBtn_;
  QPushButton* takeSpectatorBtn_;
  QPushButton* readyBtn_;

  QLabel* blackLabel_ = nullptr;
  QLabel* whiteLabel_ = nullptr;
  QLabel* blackIcon_ = nullptr;
  QLabel* whiteIcon_ = nullptr;
  QLabel* spectatorsLabel_ = nullptr;

  QString roomId_;
  QString lastReqId_;
  QString myRole_; // "black" | "white" | "spectator" | ""
  QString blackUser_;
  QString whiteUser_;
  bool blackReady_ = false;
  bool whiteReady_ = false;
};

#endif // ONLINEROOMDIALOG_H
