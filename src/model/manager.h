#ifndef MANAGER_H
#define MANAGER_H

#include "coordinate.h"
#include "match.h"
#include "player.h"

class Manager : public QObject {
  Q_OBJECT

  friend class Player;

public:
  Manager();

    Unit getCurColor() const;
    Unit getWinner() const;
    Coordinate getLatestCoord() const;
    QVector<Coordinate> getUndoList() const;

    int getAiDelayMs() const { return aiDelayMs; }

  static Manager *getInstance() {
    static Manager *singleton = nullptr;
    if (!singleton)
      singleton = new Manager();
    return singleton;
  }

  // 由外部提供两侧玩家（可以是本地/AI/网络等任意实现），Manager 只负责回合调度与胜负收敛。
  void setPlayers(Player *black, Player *white);
  bool isLocalTurn() const;

signals:

  void onOverlap();
  void onDropped();
  void onGameOver();
  void onUndoDone();

public slots:

    void drop(Coordinate);
    void blackUndo();
    void whiteUndo();
    void restart();

    void setAiDelayMs(int delayMs);

private:
  Match *match;
  Player *blackPlayer = nullptr;
  Player *whitePlayer = nullptr;
  int aiDelayMs = 250;

private slots:
  void onPlayerMove(Coordinate coord);
  void onMatchDropped();
  void onMatchEnded();
  void onMatchUndoDone();
  void onMatchOverlap();

private:
  const Board &boardView() const { return match->boardView(); }
  void syncPlayersToMatch();
  void notifyMoveApplied(Coordinate coord, Unit unit);
  void notifyMoveReverted(Coordinate coord);
  Player *currentPlayer() const;
  void requestTurn();

};

#endif // MANAGER_H
