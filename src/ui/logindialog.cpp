#include "logindialog.h"

#include "../net/authapi.h"
#include "../net/authstore.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("登录");
  setModal(true);
  setFixedSize(QSize(420, 240));

  auto mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(18, 18, 18, 18);
  mainLayout->setSpacing(12);
  setLayout(mainLayout);

  title_ = new QLabel("联机登录");
  title_->setAlignment(Qt::AlignHCenter);
  title_->setStyleSheet("font-size: 18px; font-weight: 600;");
  mainLayout->addWidget(title_);

  serverLabel_ = new QLabel(QString("服务器：%1").arg(AuthStore::serverBaseUrl()));
  serverLabel_->setStyleSheet("font-size: 12px; color: #666;");
  mainLayout->addWidget(serverLabel_);

  usernameEdit_ = new QLineEdit();
  usernameEdit_->setPlaceholderText("用户名");
  usernameEdit_->setMinimumHeight(32);
  usernameEdit_->setText(AuthStore::username());
  usernameEdit_->setStyleSheet("font-size: 14px;");
  mainLayout->addWidget(usernameEdit_);

  passwordEdit_ = new QLineEdit();
  passwordEdit_->setPlaceholderText("密码");
  passwordEdit_->setMinimumHeight(32);
  passwordEdit_->setEchoMode(QLineEdit::Password);
  passwordEdit_->setStyleSheet("font-size: 14px;");
  mainLayout->addWidget(passwordEdit_);

  statusLabel_ = new QLabel();
  statusLabel_->setStyleSheet("font-size: 12px; color: #B00020;");
  statusLabel_->setWordWrap(true);
  mainLayout->addWidget(statusLabel_);

  auto buttonRow = new QHBoxLayout();
  buttonRow->addStretch(1);
  loginBtn_ = new QPushButton("登录");
  registerBtn_ = new QPushButton("注册");
  cancelBtn_ = new QPushButton("取消");
  loginBtn_->setMinimumSize(QSize(96, 34));
  registerBtn_->setMinimumSize(QSize(96, 34));
  cancelBtn_->setMinimumSize(QSize(96, 34));
  loginBtn_->setStyleSheet("font-size: 14px;");
  registerBtn_->setStyleSheet("font-size: 14px;");
  cancelBtn_->setStyleSheet("font-size: 14px;");
  buttonRow->addWidget(loginBtn_);
  buttonRow->addWidget(registerBtn_);
  buttonRow->addWidget(cancelBtn_);
  mainLayout->addLayout(buttonRow);

  connect(cancelBtn_, &QPushButton::clicked, this, &QDialog::reject);
  connect(loginBtn_, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
  connect(registerBtn_, &QPushButton::clicked, this,
          &LoginDialog::onRegisterClicked);
  connect(passwordEdit_, &QLineEdit::returnPressed, this,
          &LoginDialog::onLoginClicked);

  auto api = AuthApi::getInstance();
  connect(api, &AuthApi::loginSucceeded, this, &LoginDialog::onLoginSucceeded,
          Qt::UniqueConnection);
  connect(api, &AuthApi::loginFailed, this, &LoginDialog::onLoginFailed,
          Qt::UniqueConnection);
  connect(api, &AuthApi::registerFailed, this, &LoginDialog::onRegisterFailed,
          Qt::UniqueConnection);
}

void LoginDialog::onLoginClicked() {
  statusLabel_->clear();

  const auto username = usernameEdit_->text().trimmed();
  const auto password = passwordEdit_->text();

  if (username.isEmpty() || password.isEmpty()) {
    statusLabel_->setText("请输入用户名和密码");
    return;
  }

  loginBtn_->setEnabled(false);
  registerBtn_->setEnabled(false);
  cancelBtn_->setEnabled(false);
  statusLabel_->setText("正在登录…");

  AuthApi::getInstance()->login(username, password);
}

void LoginDialog::onRegisterClicked() {
  statusLabel_->clear();

  const auto username = usernameEdit_->text().trimmed();
  const auto password = passwordEdit_->text();

  if (username.isEmpty() || password.isEmpty()) {
    statusLabel_->setText("请输入用户名和密码");
    return;
  }

  loginBtn_->setEnabled(false);
  registerBtn_->setEnabled(false);
  cancelBtn_->setEnabled(false);
  statusLabel_->setText("正在注册…");

  AuthApi::getInstance()->registerAndLogin(username, password);
}

void LoginDialog::onLoginSucceeded(const QString& username) {
  Q_UNUSED(username);
  accept();
}

void LoginDialog::onLoginFailed(const QString& message) {
  loginBtn_->setEnabled(true);
  registerBtn_->setEnabled(true);
  cancelBtn_->setEnabled(true);
  statusLabel_->setText(message);
}

void LoginDialog::onRegisterFailed(const QString& message) {
  loginBtn_->setEnabled(true);
  registerBtn_->setEnabled(true);
  cancelBtn_->setEnabled(true);
  statusLabel_->setText(message);
}
