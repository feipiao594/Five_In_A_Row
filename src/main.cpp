#include "ui/mainui.h"
#include "ui/selectionui.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  SelectionUI::getInstance()->show();
  qDebug() << "hello";
  return a.exec();
}
