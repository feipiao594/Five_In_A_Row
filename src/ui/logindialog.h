#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoginDialog(QWidget* parent = nullptr);

private slots:
  void onLoginClicked();
  void onRegisterClicked();
  void onLoginSucceeded(const QString& username);
  void onLoginFailed(const QString& message);
  void onRegisterFailed(const QString& message);

private:
  QLabel* title_;
  QLabel* serverLabel_;
  QLineEdit* usernameEdit_;
  QLineEdit* passwordEdit_;
  QPushButton* loginBtn_;
  QPushButton* registerBtn_;
  QPushButton* cancelBtn_;
  QLabel* statusLabel_;
};

#endif // LOGINDIALOG_H
