#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QTimer>

#include "board.h"
#include "computer.h"
#include "coordinate.h"
#include "resource.h"

class Board;
class Manager;

class Player : public QObject {
  Q_OBJECT

public:
  explicit Player(Unit unit, Manager *manager, QObject *parent = nullptr)
      : QObject(parent), manager_(manager), unit_(unit) {}

  Unit unit() const { return unit_; }

  virtual bool requiresLocalInput() const = 0;
  virtual void requestTurn() = 0;
  virtual void reset() {}

  // 由 Manager 在 Match 状态变化时调用，用于玩家（尤其是 AI）同步内部状态。
  virtual void onCoreReset() {}
  virtual void onCoreMoveApplied(Coordinate, Unit) {}
  virtual void onCoreMoveReverted(Coordinate) {}

protected:
  Manager *manager() const { return manager_; }
  const Board &board() const;

signals:
  void moveReady(Coordinate coord);

private:
  Manager *manager_;
  Unit unit_;
};

class HumanPlayer : public Player {
  Q_OBJECT

public:
  explicit HumanPlayer(Unit unit, Manager *manager, QObject *parent = nullptr)
      : Player(unit, manager, parent) {}

  bool requiresLocalInput() const override { return true; }
  void requestTurn() override { waiting_ = true; }
  void reset() override { waiting_ = false; }

public slots:
  void onBoardClick(int x, int y) {
    if (!waiting_)
      return;
    waiting_ = false;
    emit moveReady(Coordinate(y, x));
  }

private:
  bool waiting_ = false;
};

class AiPlayer : public Player {
  Q_OBJECT

public:
  AiPlayer(Unit unit, Manager *manager, QObject *parent = nullptr)
      : Player(unit, manager, parent), analysisBoard_(), computer_(analysisBoard_) {}

  bool requiresLocalInput() const override { return false; }
  void requestTurn() override;

  void onCoreReset() override;
  void onCoreMoveApplied(Coordinate coord, Unit unit) override;
  void onCoreMoveReverted(Coordinate coord) override;

private:
  Board analysisBoard_;
  Computer computer_;
};

class NetworkPlayer : public Player {
  Q_OBJECT

public:
  explicit NetworkPlayer(Unit unit, Manager *manager, QObject *parent = nullptr)
      : Player(unit, manager, parent) {}

  bool requiresLocalInput() const override { return false; }
  void requestTurn() override { waiting_ = true; }
  void reset() override { waiting_ = false; }

public slots:
  void onRemoteMove(Coordinate coord) {
    if (!waiting_)
      return;
    waiting_ = false;
    emit moveReady(coord);
  }

private:
  bool waiting_ = false;
};

#endif // PLAYER_H
