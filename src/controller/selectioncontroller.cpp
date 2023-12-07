#include "selectioncontroller.h"
#include "../model/manager.h"

void SelectionController::selectAImode(Unit color) {
  if (color == Unit::Black)
    Manager::getInstance()->setComputer(false, true);
  else
    Manager::getInstance()->setComputer(true, false);
};

void SelectionController::selectPersonmode() {
  Manager::getInstance()->setComputer(false, false);
};
