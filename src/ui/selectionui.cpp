#include "selectionui.h"
#include "mainui.h"

#include <QMessageBox>
#include <QMouseEvent>

SelectionUI::SelectionUI() : mainLayout(new QVBoxLayout),
      personModeButton(new QPushButton("双人游戏")), aiModeButton(new QPushButton("人机对战")) {
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(200,150));
    setLayout(mainLayout);
    mainLayout->setAlignment(Qt::AlignHCenter);
    mainLayout->setSpacing(35);
    personModeButton->setFixedWidth(90);
    aiModeButton->setFixedWidth(90);
    mainLayout->addWidget(personModeButton);
    mainLayout->addWidget(aiModeButton);
    connect(personModeButton,&QPushButton::clicked,this,&SelectionUI::clickedAiMode);
    connect(aiModeButton,&QPushButton::clicked,this,&SelectionUI::clickedPersonMode);

    /////////////////TEST Connect
    connect(this,&SelectionUI::clickedAiMode,this,&SelectionUI::startGame);
    connect(this,&SelectionUI::clickedPersonMode,this,&SelectionUI::startGame);
    /////////////////TEST
}

void SelectionUI::startGame() {
    hide();
    MainUI::getInstance()->show();
};

void SelectionUI::showSelection() {
    MainUI::getInstance()->hide();
    show();
};
