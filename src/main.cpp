#include "ui/menuui.h"

#include <QApplication>
//
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MenuUI::getInstance()->show();
  return a.exec();
}
