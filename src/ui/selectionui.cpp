#include "selectionui.h"
#include "../controller/selectioncontroller.h"
#include "../model/manager.h"
#include "mainui.h"

#include <QMessageBox>
#include <QMouseEvent>

SelectionUI::SelectionUI()
    : mainLayout(new QVBoxLayout),
      personModeButton(new QPushButton("双人游戏")),
      aiModeButton(new QPushButton("人机对战")) {
  setAttribute(Qt::WA_DeleteOnClose);
  setFixedSize(QSize(200, 150));
  setLayout(mainLayout);
  mainLayout->setAlignment(Qt::AlignHCenter);
  mainLayout->setSpacing(35);
  personModeButton->setFixedWidth(90);
  aiModeButton->setFixedWidth(90);
  mainLayout->addWidget(personModeButton);
  mainLayout->addWidget(aiModeButton);

  Manager::getInstance()->setParent(this);
  Resource::getInstance()->setParent(this);
  SelectionController::getInstance()->setParent(this);

  connect(personModeButton, &QPushButton::clicked, this,
          &SelectionUI::startGamePerson);
  connect(aiModeButton, &QPushButton::clicked, this, &SelectionUI::startGameAI);
  connect(this, &SelectionUI::clickedAiMode, SelectionController::getInstance(),
          &SelectionController::selectAImode);
  connect(this, &SelectionUI::clickedPersonMode,
          SelectionController::getInstance(),
          &SelectionController::selectPersonmode);
  connect(SelectionController::getInstance(),
          &SelectionController::updateAiMode, MainUI::getInstance(),
          &MainUI::changeAiModeView);
}

void SelectionUI::startGame() {
  hide();
  MainUI::getInstance()->show();
}

void SelectionUI::startGamePerson() {
  emit clickedPersonMode();
  startGame();
};

void SelectionUI::startGameAI() {
  QPushButton blackIsPerson;
  QPushButton whiteIsPerson;

  blackIsPerson.setText("黑方(先手)");
  whiteIsPerson.setText("白方(后手)");

  blackIsPerson.setIcon(
      Resource::getInstance()->color2pixmap(Unit::Black, false));
  whiteIsPerson.setIcon(
      Resource::getInstance()->color2pixmap(Unit::White, false));

  QMessageBox question;
  question.setText("请选择您是哪一方，另一方则为人机");
  question.setWindowTitle("选择你的颜色");
  question.setIcon(QMessageBox::Information);
  auto blackButton = question.addButton("黑方(先手)", QMessageBox::ActionRole);
  auto whitekButton = question.addButton("白方(后手)", QMessageBox::ActionRole);
  question.exec();

  if (question.clickedButton() == blackButton)
    emit clickedAiMode(Unit::Black);
  else
    emit clickedAiMode(Unit::White);

  startGame();
}

void SelectionUI::showSelection() {
  MainUI::getInstance()->hide();
  show();
}
