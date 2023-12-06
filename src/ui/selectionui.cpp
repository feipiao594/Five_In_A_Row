#include "selectionui.h"
#include "mainui.h"

#include <QMessageBox>
#include <QMouseEvent>

SelectionUI::SelectionUI() : mainLayout(new QVBoxLayout), aiModeButton(new QPushButton("双人游戏")),
      personModeButton(new QPushButton("人机对战")) {
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(200,150));
    setLayout(mainLayout);
    mainLayout->addWidget(personModeButton);
    mainLayout->addWidget(aiModeButton);
    connect(personModeButton,&QPushButton::clicked,this,&SelectionUI::clickedAiMode);
    connect(aiModeButton,&QPushButton::clicked,this,&SelectionUI::clickedPersonMode);

    /////////////////TEST Connect
    connect(this,&SelectionUI::clickedAiMode,this,&SelectionUI::startGame);
    connect(this,&SelectionUI::clickedPersonMode,this,&SelectionUI::startGame);
    /////////////////TEST
}

void SelectionUI::endProgram() {
    //qDebug()<<"close SelectionUI";
    close();
}

void SelectionUI::startGame() {
    hide();
    MainUI::getInstance()->show();
};

void SelectionUI::showSelection() {
    MainUI::getInstance()->hide();
    show();
};
