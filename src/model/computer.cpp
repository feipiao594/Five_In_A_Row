#include <QStack>
#include <random>

#include "computer.h"

Computer::Computer() { clear(); }

Computer::Computer(Board &board) {
  this->board = &board;
  clear();
}

void Computer::clear() {
  for (int r = 0; r < BOARD_SIZE - 4; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      verGroup[r][c][0] = verGroup[r][c][1] = 0;
      horGroup[c][r][0] = horGroup[c][r][1] = 0;
    }
    for (int c = 0; c < BOARD_SIZE - 4; c++) {
      posGroup[r][c][0] = posGroup[r][c][1] = 0;
      negGroup[r][c][0] = negGroup[r][c][1] = 0;
    }
  }
}

void Computer::fitGroup(Coordinate coord, int unit, int weight) {
  fitHorGroup(coord, unit, weight);
  fitVerGroup(coord, unit, weight);
  fitPosGroup(coord, unit, weight);
  fitNegGroup(coord, unit, weight);
}

void Computer::fitHorGroup(Coordinate coord, int unit, int weight) {
  int startCol = std::max(coord.col - 4, 0);
  int endCol = std::min(coord.col, BOARD_SIZE - 1 - 4);
  for (int c = startCol; c <= endCol; c++)
    horGroup[coord.row][c][unit] += weight;
}

void Computer::fitVerGroup(Coordinate coord, int unit, int weight) {
  int startRow = std::max(coord.row - 4, 0);
  int endRow = std::min(coord.row, BOARD_SIZE - 1 - 4);
  for (int r = startRow; r <= endRow; r++)
    verGroup[r][coord.col][unit] += weight;
}

void Computer::fitPosGroup(Coordinate coord, int unit, int weight) {
  int startRow = std::min(coord.row + 4, BOARD_SIZE - 1) - 4;
  int col = coord.col + coord.row - 4 - startRow;
  int endRow = std::max(coord.row - 4, 0);
  for (int r = startRow; r >= endRow; r--) {
    if (col >= 0 && col < BOARD_SIZE - 4)
      posGroup[r][col][unit] += weight;
    col++;
  }
}

void Computer::fitNegGroup(Coordinate coord, int unit, int weight) {
  int startRow = std::max(coord.row - 4, 0);
  int col = coord.col - coord.row + startRow;
  int endRow = std::min(coord.row, BOARD_SIZE - 1 - 4);
  for (int r = startRow; r <= endRow; r++) {
    if (col >= 0 && col < BOARD_SIZE - 4)
      negGroup[r][col][unit] += weight;
    col++;
  }
}

void Computer::update(Coordinate coord, Unit unit) {
  fitGroup(coord, unit == Unit::Black ? 0 : 1, 1);
}

void Computer::remove(Coordinate coord, Unit unit) {
  fitGroup(coord, unit == Unit::Black ? 0 : 1, -1);
}

int Computer::count2Score(int blackNum, int whiteNum, Unit unit) {
  if (blackNum == 0 && whiteNum == 0) return 7;
  if (blackNum > 0 && whiteNum > 0) return 0;

  int local = unit == Unit::Black ? blackNum : whiteNum;
  int rival = unit == Unit::Black ? whiteNum : blackNum;

  if (rival == 0) {
    if (local == 1) return 35;
    if (local == 2) return 800;
    if (local == 3) return 15000;
    return 800000;
  } else {
    if (rival == 1) return 15;
    if (rival == 2) return 400;
    if (rival == 3) return 2800;
    return 100000;
  }
}

int Computer::horGroupScore(Coordinate coord, Unit unit) {
  int score = 0;
  int startCol = std::max(coord.col - 4, 0);
  int endCol = std::min(coord.col, BOARD_SIZE - 1 - 4);
  for (int c = startCol; c <= endCol; c++)
    score += count2Score(horGroup[coord.row][c][0], horGroup[coord.row][c][1], unit);
  return score;
}

int Computer::verGroupScore(Coordinate coord, Unit unit) {
  int score = 0;
  int startRow = std::max(coord.row - 4, 0);
  int endRow = std::min(coord.row, BOARD_SIZE - 1 - 4);
  for (int r = startRow; r <= endRow; r++)
    score += count2Score(verGroup[r][coord.col][0], verGroup[r][coord.col][1], unit);
  return score;
}

int Computer::posGroupScore(Coordinate coord, Unit unit) {
  int score = 0;
  int startRow = std::min(coord.row + 4, BOARD_SIZE - 1) - 4;
  int col = coord.col + coord.row - 4 - startRow;
  int endRow = std::max(coord.row - 4, 0);
  for (int r = startRow; r >= endRow; r--) {
    if (col >= 0 && col < BOARD_SIZE - 4)
      score += count2Score(posGroup[r][col][0], posGroup[r][col][1], unit);
    col++;
  }
  return score;
}

int Computer::negGroupScore(Coordinate coord, Unit unit) {
  int score = 0;
  int startRow = std::max(coord.row - 4, 0);
  int col = coord.col - coord.row + startRow;
  int endRow = std::min(coord.row, BOARD_SIZE - 1 - 4);
  for (int r = startRow; r <= endRow; r++) {
    if (col >= 0 && col < BOARD_SIZE - 4)
      score += count2Score(negGroup[r][col][0], negGroup[r][col][1], unit);
    col++;
  }
  return score;
}

int Computer::getRandomInt(int range) {
  std::random_device seed;
  std::ranlux48 engine(seed());
  std::uniform_int_distribution<> distrib(0, range);
  return distrib(engine);
}

int Computer::selectCandidates(QVector<std::pair<Coordinate, int>>& list, Unit unit) {
  int maxScore = 0;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (board->units[r][c] != Unit::Empty)
          continue;
      Coordinate coord = Coordinate(r, c);
      int score = horGroupScore(coord, unit) + verGroupScore(coord, unit) +
                  posGroupScore(coord, unit) + negGroupScore(coord, unit);
      if (score >= maxScore) {
          maxScore = score;
          list.push_back(std::make_pair(coord, score));
      }
    }
  }
  return maxScore;
}

Coordinate Computer::getBestCoord(Unit unit) {
  QVector<std::pair<Coordinate, int>> candidateList;
  int maxScore = selectCandidates(candidateList, unit);
  if (candidateList.empty())
    return Coordinate(-1, -1);
  for (int i = (int)candidateList.size() - 1; i >= 0; i--) {
    if (candidateList.at(i).second < maxScore)
      candidateList.remove(i);
  }
  return candidateList.at(getRandomInt((int)candidateList.size() - 1)).first;
}

Coordinate Computer::getGameCoord(Unit unit) {
  QVector<std::pair<Coordinate, int>> candidateList;
  int maxScore = selectCandidates(candidateList, unit);
  if (candidateList.empty())
    return Coordinate(-1, -1);

  for (int i = (int)candidateList.size() - 1; i >= 0; i--) {
    if (candidateList.at(i).second < maxScore)
      candidateList.remove(i);
  }

  int maxWeight = INT32_MIN;
  QStack<Coordinate> record;
  for (int i = (int)candidateList.size() - 1; i >= 0; i--) {

    Coordinate coord = candidateList.at(i).first;
    Unit gameUnit = unit;
    int localScore = maxScore, rivalScore = 0;
    int score = maxScore;

    while (score < 800000) {
      board->setUnit(coord, gameUnit);
      update(coord, gameUnit);
      record.push(coord);
      gameUnit = (Unit)-(int)gameUnit;

      QVector<std::pair<Coordinate, int>> list;
      score = selectCandidates(list, unit);
      (gameUnit == unit ? localScore : rivalScore) = score;

      if (list.empty()) break;
      for (int it = (int)list.size() - 1; it >= 0; it--) {
        if (list.at(it).second < score)
          list.remove(it);
      }

      coord = list.at(getRandomInt((int)list.size() - 1)).first;
    }

    while (!record.empty()) {
      gameUnit = (Unit)-(int)gameUnit;
      coord = record.pop();
      board->setUnit(coord, Unit::Empty);
      remove(coord, gameUnit);
    }

    int weight = localScore - rivalScore;
    candidateList[i].second = weight;
    if (weight >= maxWeight) maxWeight = weight;
    else candidateList.remove(i);
  }

  for (int i = (int)candidateList.size() - 1; i >= 0; i--) {
    if (candidateList.at(i).second < maxWeight)
      candidateList.remove(i);
  }
  return candidateList.at(getRandomInt((int)candidateList.size() - 1)).first;
}
