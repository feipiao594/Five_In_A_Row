#ifndef ONLINESESSION_H
#define ONLINESESSION_H

#include <QObject>
#include <QJsonObject>

#include "../model/resource.h"

class OnlineSession : public QObject {
  Q_OBJECT

public:
  static OnlineSession* getInstance();

  void start();
  void stop();
  bool isActive() const { return active_; }

signals:
  void stateChanged();

private:
  explicit OnlineSession(QObject* parent = nullptr);

  void bindSignals();
  void unbindSignals();

  void onWsEvent(const QString& type, const QJsonObject& payload);
  void onWsResp(const QString& type, const QString& reqId, bool ok,
                const QJsonObject& payload, const QString& errorMessage,
                const QString& errorCode);
  void handleRoomSnapshot(const QJsonObject& payload);
  void handleMatchStart(const QJsonObject& payload);
  void handleMatchMoved(const QJsonObject& payload);
  void handleMatchOver(const QJsonObject& payload);

  void onBoardClicked(int x, int y);

  bool active_ = false;
  QString roomId_;
  QString matchId_;
  Unit localColor_ = Unit::Empty;
  Unit turn_ = Unit::Black;
};

#endif // ONLINESESSION_H
