#include "ui/menuui.h"

#include <QApplication>
#include <QCoreApplication>

#include "net/authapi.h"
#include "net/authstore.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QCoreApplication::setApplicationName("FIVE_IN_A_ROW");
  QCoreApplication::setOrganizationName("FiveInARow");
  MenuUI::getInstance()->show();
  if (AuthStore::isLoggedIn())
    AuthApi::getInstance()->checkSession();
  return a.exec();
}
