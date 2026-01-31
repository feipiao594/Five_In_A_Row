#include "ui/menuui.h"

#include <QApplication>

#include "net/authapi.h"
#include "net/authstore.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MenuUI::getInstance()->show();
  if (AuthStore::isLoggedIn())
    AuthApi::getInstance()->checkSession();
  return a.exec();
}
