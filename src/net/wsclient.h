#ifndef WSCLIENT_H
#define WSCLIENT_H

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QUrl>

class QWebSocket;

class WsClient : public QObject {
  Q_OBJECT

public:
  static WsClient* getInstance();

  bool isConnected() const;
  void connectIfNeeded();
  void disconnectNow();

  QString sendRequest(const QString& type, const QJsonObject& payload);
  void sendEvent(const QString& type, const QJsonObject& payload);

signals:
  void connected();
  void disconnected();
  void errorOccurred(const QString& message);

  void eventReceived(const QString& type, const QJsonObject& payload);
  void responseReceived(const QString& type, const QString& reqId, bool ok,
                        const QJsonObject& payload, const QString& errorMessage,
                        const QString& errorCode);

private:
  explicit WsClient(QObject* parent = nullptr);
  void openSocket(const QUrl& url);
  void handleTextMessage(const QString& text);
  static QUrl toWsUrl(const QString& httpBase, const QString& accessToken);

  QWebSocket* ws_;
  bool connecting_ = false;
};

#endif // WSCLIENT_H
