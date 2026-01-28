#include "player.h"
#include "manager.h"

const Board& Player::board() const {
    return manager()->boardView();
}

void AiPlayer::requestTurn() {
    QTimer::singleShot(manager()->getAiDelayMs(), this,
                       [this]() { emit moveReady(computer_.getGameCoord(unit())); });
}

void AiPlayer::onCoreReset() {
    analysisBoard_.clear();
    computer_.clear();
}

void AiPlayer::onCoreMoveApplied(Coordinate coord, Unit unit) {
    analysisBoard_.setUnit(coord, unit);
    computer_.update(coord, unit);
}

void AiPlayer::onCoreMoveReverted(Coordinate coord) {
    Unit unit = analysisBoard_.getUnit(coord);
    if (unit == Unit::Empty)
        return;
    analysisBoard_.setUnit(coord, Unit::Empty);
    computer_.remove(coord, unit);
}
