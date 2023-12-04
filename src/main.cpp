#include "ui.h"
#include "record.h"

#include <QApplication>

Board board;
Record record;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUI w;
    w.show();
    return a.exec();
}
