#include "manager.h"

Coordinate dirVector[4] = {{0,1}, {1,0}, {-1,1}, {1,1}};

Player Manager::getCurPlayer() { return ((getTotalStep() & 1) == 0) ? black : white; }

int Manager::getTotalStep() { return record.size(); }

Unit Manager::getCurColor() { return getCurPlayer().unit; }

Unit Manager::getWinner() { return winner; }

Manager::Manager(bool isBlackComputer, bool isWhiteComputer) {
  board = Board();
  black = Player(Unit::Black, isBlackComputer);
  white = Player(Unit::White, isWhiteComputer);
  record = QStack<Coordinate>();
}

bool Manager::isWin(Coordinate baseCoord) {
  Unit unit = board.getUnit(baseCoord);
  for (int i = 0; i < 4; i++) {
    Coordinate dirCoord = dirVector[i];
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

void Manager::drop(Coordinate coord)
{
    if (board.getUnit(coord) == Unit::Empty)
    {
        Player player = getCurPlayer();
        board.setUnit(coord, player.unit);
        record.push(coord);
        if (isWin(coord))
        {
            winner = player.unit;
            emit onGameOver();
        }
        else if (getTotalStep() >= BOARD_SIZE * BOARD_SIZE)
        {
            winner = Unit::Empty;
            emit onGameOver();
        }
        else
        {
            player = getCurPlayer();
            emit onDropped();
        }
    }
    else emit onOverlap();
}

void Manager::undo() {
  if (!record.empty())
    board.setUnit(record.pop(), Unit::Empty);
}

void Manager::blackUndo() {
  if (getCurPlayer().unit == Unit::Black)
    undo();
  undo();
}

void Manager::whiteUndo() {
  if (getCurPlayer().unit == Unit::White)
    undo();
  undo();
}

Coordinate Manager::compute(Unit unit)
{
    return Coordinate();
}

