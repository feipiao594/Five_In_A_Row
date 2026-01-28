#include "menuui.h"
#include "mainui.h"
#include "settingsdialog.h"

#include "../controller/selectioncontroller.h"

#include <QMessageBox>

MenuUI::MenuUI()
    : mainLayout(new QVBoxLayout),
      title(new QLabel("五子棋")),
      modeGroup(new QGroupBox("模式")),
      localModeRadio(new QRadioButton("单机模式")),
      onlineModeRadio(new QRadioButton("联机模式（暂未实现）")),
      roomGroup(new QGroupBox("房间设置")),
      blackType(new QComboBox),
      whiteType(new QComboBox),
      startButton(new QPushButton("开始")),
      settingsButton(new QPushButton("设置")),
      exitButton(new QPushButton("退出")) {
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle("菜单");
  setFixedSize(QSize(420, 400));
  setLayout(mainLayout);

  Resource::getInstance()->setParent(this);
  SelectionController::getInstance()->setParent(this);

  title->setAlignment(Qt::AlignHCenter);
  title->setStyleSheet("font-size: 26px; font-weight: 400;");
  mainLayout->setContentsMargins(18, 18, 18, 18);
  mainLayout->setSpacing(20);
  mainLayout->addWidget(title);

  // mode group
  auto modeLayout = new QVBoxLayout();
  modeGroup->setLayout(modeLayout);
  modeGroup->setStyleSheet("QGroupBox{font-size:14px;font-weight:600;}");
  localModeRadio->setStyleSheet("font-size: 14px;");
  onlineModeRadio->setStyleSheet("font-size: 14px;");
  localModeRadio->setMinimumHeight(28);
  onlineModeRadio->setMinimumHeight(28);
  modeLayout->addWidget(localModeRadio);
  modeLayout->addWidget(onlineModeRadio);
  localModeRadio->setChecked(true);
  mainLayout->addWidget(modeGroup);

  // room group
  auto roomLayout = new QFormLayout();
  roomGroup->setLayout(roomLayout);
  roomGroup->setStyleSheet("QGroupBox{font-size:14px;font-weight:600;}");
  roomLayout->setLabelAlignment(Qt::AlignLeft);
  roomLayout->setFormAlignment(Qt::AlignLeft);
  roomLayout->setHorizontalSpacing(14);
  roomLayout->setVerticalSpacing(10);
  blackType->setMinimumHeight(32);
  whiteType->setMinimumHeight(32);
  blackType->setMinimumWidth(160);
  whiteType->setMinimumWidth(160);
  blackType->setStyleSheet("font-size: 14px;");
  whiteType->setStyleSheet("font-size: 14px;");
  blackType->addItem("本地玩家");
  blackType->addItem("电脑");
  whiteType->addItem("本地玩家");
  whiteType->addItem("电脑");
  roomLayout->addRow("黑方（先手）", blackType);
  roomLayout->addRow("白方（后手）", whiteType);
  mainLayout->addWidget(roomGroup);

  // buttons
  auto buttonRow = new QHBoxLayout();
  buttonRow->addStretch(1);
  startButton->setMinimumSize(QSize(96, 34));
  settingsButton->setMinimumSize(QSize(96, 34));
  exitButton->setMinimumSize(QSize(96, 34));
  startButton->setStyleSheet("font-size: 14px;");
  settingsButton->setStyleSheet("font-size: 14px;");
  exitButton->setStyleSheet("font-size: 14px;");
  buttonRow->addWidget(startButton);
  buttonRow->addWidget(settingsButton);
  buttonRow->addWidget(exitButton);
  mainLayout->addLayout(buttonRow);

  connect(exitButton, &QPushButton::clicked, this, &MenuUI::endProgram);
  connect(startButton, &QPushButton::clicked, this, &MenuUI::onStartClicked);
  connect(settingsButton, &QPushButton::clicked, this,
          &MenuUI::onSettingsClicked);

  connect(localModeRadio, &QRadioButton::toggled, this, &MenuUI::onModeChanged);
  connect(onlineModeRadio, &QRadioButton::toggled, this, &MenuUI::onModeChanged);

  connect(this, &MenuUI::startLocal, SelectionController::getInstance(),
          &SelectionController::startLocal);
  connect(this, &MenuUI::startOnline, SelectionController::getInstance(),
          &SelectionController::startOnline);
  connect(SelectionController::getInstance(),
          &SelectionController::updateComputerSides, MainUI::getInstance(),
          &MainUI::setComputerSides);
  connect(MainUI::getInstance(), &MainUI::mainUIClosed, this,
          &MenuUI::showMenu);

  onModeChanged();
}

void MenuUI::startGame() {
  hide();
  MainUI::getInstance()->show();
}

void MenuUI::onModeChanged() {
  bool isLocal = localModeRadio->isChecked();
  roomGroup->setEnabled(isLocal);
}

void MenuUI::onStartClicked() {
  if (onlineModeRadio->isChecked()) {
    QMessageBox::information(this, "提示", "联机模式暂未实现。");
    emit startOnline();
    return;
  }

  bool blackIsComputer = (blackType->currentIndex() == 1);
  bool whiteIsComputer = (whiteType->currentIndex() == 1);
  emit startLocal(blackIsComputer, whiteIsComputer);
  MainUI::getInstance()->prepareNewGame();
  startGame();
}

void MenuUI::onSettingsClicked() {
  SettingsDialog dialog(this);
  dialog.exec();
}

void MenuUI::showMenu() {
  MainUI::getInstance()->hide();
  show();
}

