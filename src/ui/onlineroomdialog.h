#ifndef ONLINEROOMDIALOG_H
#define ONLINEROOMDIALOG_H

#include <QDialog>
#include <QJsonObject>

class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

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
  void onWsError(const QString& message);
  void onWsEvent(const QString& type, const QJsonObject& payload);
  void onWsResp(const QString& type, const QString& reqId, bool ok,
                const QJsonObject& payload, const QString& errorMessage,
                const QString& errorCode);

private:
  void setStatus(const QString& s);
  void refreshButtons();
  void renderSnapshot(const QJsonObject& snap);
  void updateRoleFromSnapshot(const QJsonObject& snap);

  QLabel* title_;
  QLabel* status_;
  QLabel* roleLabel_;
  QPushButton* connectBtn_;

  QLabel* roomLabel_;
  QLineEdit* roomIdEdit_;
  QPushButton* createBtn_;
  QPushButton* joinBtn_;

  QPushButton* takeBlackBtn_;
  QPushButton* takeWhiteBtn_;
  QPushButton* takeSpectatorBtn_;
  QPushButton* readyBtn_;

  QTextEdit* snapshotView_;

  QString roomId_;
  QString lastReqId_;
  QString myRole_; // "black" | "white" | "spectator" | ""
  QString blackUser_;
  QString whiteUser_;
};

#endif // ONLINEROOMDIALOG_H
