#include "settingsdialog.h"

#include "../net/authapi.h"
#include "../net/authstore.h"
#include "../model/manager.h"

static void updateLoginStatus(QLabel* label, QPushButton* logoutBtn) {
    if (AuthStore::isLoggedIn()) {
        label->setText(QString("已登录：%1").arg(AuthStore::username()));
        logoutBtn->setEnabled(true);
    } else {
        label->setText("未登录");
        logoutBtn->setEnabled(false);
    }
}

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent), mainLayout(new QVBoxLayout), title(new QLabel("设置")),
      aiDelayLabel(new QLabel("电脑落子延迟 (ms)")), aiDelaySpin(new QSpinBox),
      netTitle(new QLabel("联机")),
      serverLabel(new QLabel("服务器地址")),
      serverEdit(new QLineEdit),
      ignoreSslErrors(new QCheckBox("忽略 SSL 证书错误（不安全，仅用于调试）")),
      loginStatus(new QLabel), logoutButton(new QPushButton("退出登录")),
      buttonRow(new QHBoxLayout), closeButton(new QPushButton("关闭")) {
  setWindowTitle("设置");
  setModal(true);
  setFixedSize(QSize(420, 340));
  setLayout(mainLayout);

    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("font-size: 18px; font-weight: 600;");
    mainLayout->addWidget(title);

    aiDelaySpin->setRange(0, 5000);
    aiDelaySpin->setSingleStep(50);
    aiDelaySpin->setMinimumHeight(32);
    aiDelaySpin->setValue(Manager::getInstance()->getAiDelayMs());
    aiDelaySpin->setStyleSheet("font-size: 14px;");

    auto row = new QHBoxLayout();
    row->addWidget(aiDelayLabel);
    row->addStretch(1);
  row->addWidget(aiDelaySpin);
  mainLayout->addLayout(row);

  netTitle->setStyleSheet("font-size: 16px; font-weight: 600;");
  mainLayout->addWidget(netTitle);

  serverEdit->setMinimumHeight(32);
  serverEdit->setStyleSheet("font-size: 14px;");
  serverEdit->setText(AuthStore::serverBaseUrl());

  auto netRow = new QHBoxLayout();
  netRow->addWidget(serverLabel);
  netRow->addStretch(1);
  netRow->addWidget(serverEdit);
  mainLayout->addLayout(netRow);

  ignoreSslErrors->setChecked(AuthStore::ignoreSslErrors());
  ignoreSslErrors->setStyleSheet("font-size: 12px; color: #666;");
  mainLayout->addWidget(ignoreSslErrors);

  loginStatus->setStyleSheet("font-size: 12px; color: #666;");
  mainLayout->addWidget(loginStatus);

  logoutButton->setMinimumSize(QSize(96, 34));
  logoutButton->setStyleSheet("font-size: 14px;");
  mainLayout->addWidget(logoutButton, 0, Qt::AlignRight);

  updateLoginStatus(loginStatus, logoutButton);

  if (AuthStore::isLoggedIn()) {
    loginStatus->setText("正在校验登录…");
    AuthApi::getInstance()->checkSession();
  }

    buttonRow->addStretch(1);
    closeButton->setMinimumSize(QSize(96, 34));
    closeButton->setStyleSheet("font-size: 14px;");
    buttonRow->addWidget(closeButton);
    mainLayout->addLayout(buttonRow);

  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(aiDelaySpin, &QSpinBox::valueChanged, Manager::getInstance(), &Manager::setAiDelayMs);

  connect(serverEdit, &QLineEdit::editingFinished, this, [this]() {
      AuthStore::setServerBaseUrl(serverEdit->text().trimmed());
      serverEdit->setText(AuthStore::serverBaseUrl());
  });

  connect(ignoreSslErrors, &QCheckBox::toggled, this, [](bool on) {
    AuthStore::setIgnoreSslErrors(on);
  });

  connect(logoutButton, &QPushButton::clicked, this, [this]() {
      logoutButton->setEnabled(false);
      loginStatus->setText("正在退出登录…");
      AuthApi::getInstance()->logout();
  });

  connect(AuthApi::getInstance(), &AuthApi::logoutFinished, this,
          [this](bool ok, const QString& message) {
              Q_UNUSED(ok);
              loginStatus->setText(message);
              updateLoginStatus(loginStatus, logoutButton);
          });

  connect(AuthApi::getInstance(), &AuthApi::sessionChecked, this,
          [this](bool loggedIn, const QString& message) {
            Q_UNUSED(loggedIn);
            loginStatus->setText(message);
            updateLoginStatus(loginStatus, logoutButton);
          });
}
