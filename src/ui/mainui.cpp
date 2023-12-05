#include "mainui.h"
#include "selectionui.h"

MainUI::MainUI() : mainLayout(new QVBoxLayout), aiModeButton(new PieceButton(150)),
      personModeButton(new PieceButton(150)) {
    setFixedSize(QSize(800,600));
    setLayout(mainLayout);
    mainLayout->addWidget(personModeButton);
    mainLayout->addWidget(aiModeButton);
    connect(this,&MainUI::mainUIClosed,SelectionUI::getInstance(),&SelectionUI::endProgram);

    /////////////////TEST Connect
    connect(personModeButton,&QPushButton::clicked,personModeButton,[&]()-> void {
        personModeButton->setColor(OCCUPATION::BLACK);
    });
    connect(aiModeButton,&QPushButton::clicked,SelectionUI::getInstance(),&SelectionUI::showSelection);
    /////////////////TEST
}
