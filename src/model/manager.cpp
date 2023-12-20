#include "manager.h"

Coordinate dirVector[4] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

Player Manager::curPlayer() {
  return ((getTotalStep() & 1) == 0) ? black : white;
}

int Manager::getTotalStep() { return (int)record.size(); }

Unit Manager::getCurColor() { return curPlayer().unit; }

Unit Manager::getWinner() { return winner; }

Coordinate Manager::getLatestCoord() { return record.top(); }

QVector<Coordinate> Manager::getUndoList() { return undoList; }

bool Manager::getIsPerson() { return !curPlayer().isComputer; }

Manager::Manager(bool isBlackComputer, bool isWhiteComputer) {
  board = Board();
  black = Player(Unit::Black, isBlackComputer);
  white = Player(Unit::White, isWhiteComputer);
  record = QStack<Coordinate>();
  computer = Computer(board);
}

bool Manager::isWin(Coordinate baseCoord) {
  Unit unit = board.getUnit(baseCoord);
  for (int d = 0; d < 4; d++) {
    Coordinate dirCoord = dirVector[d];
    int counter = 1;
    Coordinate coord = baseCoord;
    for (int i = 0; i < 4; i++) {
      coord += dirCoord;
      if (!isCoordValid(coord) || board.getUnit(coord) != unit)
        break;
      counter++;
    }
    coord = baseCoord;
    for (int i = 0; i < 4; i++) {
      coord -= dirCoord;
      if (!isCoordValid(coord) || board.getUnit(coord) != unit)
        break;
      counter++;
    }
    if (counter >= 5)
      return true;
  }
  return false;
}

bool Manager::isCoordValid(Coordinate coord) {
  return coord.row >= 0 && coord.row < BOARD_SIZE && coord.col >= 0 &&
         coord.col < BOARD_SIZE;
}

void Manager::drop(Coordinate coord) {
  if (board.getUnit(coord) == Unit::Empty) {
    Player player = curPlayer();
    board.setUnit(coord, player.unit);
    record.push(coord);
    computer.update(coord, player.unit);
    emit onDropped();
    if (isWin(coord)) {
      winner = player.unit;
      emit onGameOver();
    } else if (getTotalStep() >= BOARD_SIZE * BOARD_SIZE) {
      winner = Unit::Empty;
      emit onGameOver();
    } else if (!getIsPerson())
      drop(compute());
  } else
    emit onOverlap();
}

Coordinate Manager::undo() {
  Coordinate coord = Coordinate(-1, -1);
  if (!record.empty()) {
    coord = record.pop();
    Unit unit = board.getUnit(coord);
    board.setUnit(coord, Unit::Empty);
    computer.remove(coord, unit);
  }
  return coord;
}

void Manager::blackUndo() {
  undoList.clear();
  if (curPlayer().unit == Unit::Black) {
    Coordinate coord = undo();
    if (coord.row >= 0)
      undoList.push_back(coord);
  }
  Coordinate coord = undo();
  if (coord.row >= 0)
    undoList.push_back(coord);
  emit onUndoDone();
  if (!getIsPerson())
    drop(compute());
}

void Manager::whiteUndo() {
  undoList.clear();
  if (curPlayer().unit == Unit::White) {
    Coordinate coord = undo();
    if (coord.row >= 0)
      undoList.push_back(coord);
  }
  Coordinate coord = undo();
  if (coord.row >= 0)
    undoList.push_back(coord);
  emit onUndoDone();
  if (!getIsPerson())
    drop(compute());
}

void Manager::restart() {
  board.clear();
  record.clear();
  computer.clear();
}

void Manager::setComputer(bool isBlackComputer, bool isWhiteComputer) {
  black.isComputer = isBlackComputer;
  white.isComputer = isWhiteComputer;

  if (curPlayer().isComputer)
    drop(compute());
}

Coordinate Manager::compute() {
//  return computer.getBestCoord(curPlayer().unit);
  return computer.getGameCoord(curPlayer().unit);
}
