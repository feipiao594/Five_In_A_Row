#include "selectioncontroller.h"
#include "../model/manager.h"
#include "boardcontroller.h"

void SelectionController::selectAImode(Unit color) {
  emit updateAiMode(color);
  if (color == Unit::Black) {
    Manager::getInstance()->setComputer(false, true);
  } else {
    Manager::getInstance()->setComputer(true, false);
  }
  BoardController::getInstance()->updateIsYourTurn();
};

void SelectionController::selectPersonmode() {
  Manager::getInstance()->setComputer(false, false);
  BoardController::getInstance()->updateIsYourTurn();
};
