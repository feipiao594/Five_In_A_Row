#include "ui/selectionui.h"
#include "ui/mainui.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SelectionUI::getInstance()->show();
    return a.exec();
}
