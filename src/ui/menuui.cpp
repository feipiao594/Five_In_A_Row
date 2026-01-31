#include "menuui.h"
#include "mainui.h"
#include "logindialog.h"
#include "settingsdialog.h"

#include "../controller/selectioncontroller.h"
#include "../net/authapi.h"
#include "../net/authstore.h"
#include "../net/onlinesession.h"
#include "../net/wsclient.h"

static void updateLoginUi(QLabel* label, QPushButton* loginBtn,
                          const QString& messageOverride = QString()) {
    if (AuthStore::isLoggedIn()) {
        loginBtn->setText("切换账号");
        label->setText(messageOverride.isEmpty()
                           ? QString("已登录：%1").arg(AuthStore::username())
                           : messageOverride);
    } else {
        loginBtn->setText("登录/注册");
        label->setText(messageOverride.isEmpty() ? "未登录" : messageOverride);
    }
}

MenuUI::MenuUI()
    : mainLayout(new QVBoxLayout), title(new QLabel("五子棋")), modeGroup(new QGroupBox("模式")),
      localModeRadio(new QRadioButton("单机模式")),
      onlineModeRadio(new QRadioButton("联机模式")),
      loginStatus(new QLabel), loginButton(new QPushButton("登录/注册")),
      roomGroup(new QGroupBox("房间设置")), blackType(new QComboBox), whiteType(new QComboBox),
      startButton(new QPushButton("开始")), settingsButton(new QPushButton("设置")),
      exitButton(new QPushButton("退出")) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("菜单");
    setFixedSize(QSize(420, 480));
    setLayout(mainLayout);

    Resource::getInstance()->setParent(this);
    SelectionController::getInstance()->setParent(this);

    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("font-size: 26px; font-weight: 400;");
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(20);
    mainLayout->addWidget(title);

    // mode group
    loginStatus->setStyleSheet("font-size: 12px; color: #666;");
    mainLayout->addWidget(loginStatus);

    auto modeLayout = new QGridLayout();
    modeGroup->setLayout(modeLayout);
    modeGroup->setStyleSheet("QGroupBox{font-size:14px;font-weight:600;}");
    localModeRadio->setStyleSheet("font-size: 14px;");
    onlineModeRadio->setStyleSheet("font-size: 14px;");
    localModeRadio->setMinimumHeight(28);
    onlineModeRadio->setMinimumHeight(28);
    modeLayout->addWidget(localModeRadio, 0, 0, 1, 2);
    modeLayout->addWidget(onlineModeRadio, 1, 0, 1, 2);
    modeLayout->setColumnStretch(0, 1);
    modeLayout->setColumnStretch(1, 0);
    modeLayout->setRowStretch(2, 1);
    loginButton->setMinimumSize(QSize(96, 34));
    loginButton->setStyleSheet("font-size: 14px;");
    modeLayout->addWidget(loginButton, 2, 1, Qt::AlignRight | Qt::AlignBottom);
    localModeRadio->setChecked(true);
    mainLayout->addWidget(modeGroup);
    updateLoginUi(loginStatus, loginButton);

    // room group
    auto roomLayout = new QFormLayout();
    roomGroup->setLayout(roomLayout);
    roomGroup->setStyleSheet("QGroupBox{font-size:14px;font-weight:600;}");
    roomLayout->setLabelAlignment(Qt::AlignLeft);
    roomLayout->setFormAlignment(Qt::AlignLeft);
    roomLayout->setHorizontalSpacing(14);
    roomLayout->setVerticalSpacing(10);
    blackType->setMinimumHeight(32);
    whiteType->setMinimumHeight(32);
    blackType->setMinimumWidth(160);
    whiteType->setMinimumWidth(160);
    blackType->setStyleSheet("font-size: 14px;");
    whiteType->setStyleSheet("font-size: 14px;");
    blackType->addItem("本地玩家");
    blackType->addItem("电脑");
    whiteType->addItem("本地玩家");
    whiteType->addItem("电脑");
    roomLayout->addRow("黑方（先手）", blackType);
    roomLayout->addRow("白方（后手）", whiteType);
    mainLayout->addWidget(roomGroup);

    // buttons
    auto buttonRow = new QHBoxLayout();
    buttonRow->addStretch(1);
    startButton->setMinimumSize(QSize(96, 34));
    settingsButton->setMinimumSize(QSize(96, 34));
    exitButton->setMinimumSize(QSize(96, 34));
    startButton->setStyleSheet("font-size: 14px;");
    settingsButton->setStyleSheet("font-size: 14px;");
    exitButton->setStyleSheet("font-size: 14px;");
    buttonRow->addWidget(startButton);
    buttonRow->addWidget(settingsButton);
    buttonRow->addWidget(exitButton);
    mainLayout->addLayout(buttonRow);

    connect(exitButton, &QPushButton::clicked, this, &MenuUI::endProgram);
    connect(startButton, &QPushButton::clicked, this, &MenuUI::onStartClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MenuUI::onSettingsClicked);

    connect(localModeRadio, &QRadioButton::toggled, this, &MenuUI::onModeChanged);
    connect(onlineModeRadio, &QRadioButton::toggled, this, &MenuUI::onModeChanged);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        LoginDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            updateLoginUi(loginStatus, loginButton);
        }
    });

    connect(AuthApi::getInstance(), &AuthApi::sessionChecked, this,
            [this](bool loggedIn, const QString& message) {
                Q_UNUSED(loggedIn);
                updateLoginUi(loginStatus, loginButton, message);
            });

    if (AuthStore::isLoggedIn()) {
        updateLoginUi(loginStatus, loginButton, "正在校验登录…");
        AuthApi::getInstance()->checkSession();
    }

    connect(this, &MenuUI::startLocal, SelectionController::getInstance(),
            &SelectionController::startLocal);
    connect(this, &MenuUI::startOnline, SelectionController::getInstance(),
            &SelectionController::startOnline);
    connect(SelectionController::getInstance(), &SelectionController::updateComputerSides,
            MainUI::getInstance(), &MainUI::setComputerSides);
    connect(MainUI::getInstance(), &MainUI::mainUIClosed, this, &MenuUI::showMenu);

    onModeChanged();
}

void MenuUI::startGame() {
    hide();
    MainUI::getInstance()->show();
}

void MenuUI::onModeChanged() {
    bool isLocal = localModeRadio->isChecked();
    roomGroup->setEnabled(isLocal);
}

void MenuUI::onStartClicked() {
    if (onlineModeRadio->isChecked()) {
        if (!AuthStore::isLoggedIn()) {
            LoginDialog dialog(this);
            if (dialog.exec() != QDialog::Accepted)
                return;
        }
        updateLoginUi(loginStatus, loginButton);
        emit startOnline();
        return;
    }

    bool blackIsComputer = (blackType->currentIndex() == 1);
    bool whiteIsComputer = (whiteType->currentIndex() == 1);
    emit startLocal(blackIsComputer, whiteIsComputer);
    MainUI::getInstance()->prepareNewGame();
    startGame();
}

void MenuUI::onSettingsClicked() {
    SettingsDialog dialog(this);
    dialog.exec();
    if (AuthStore::isLoggedIn()) {
        updateLoginUi(loginStatus, loginButton, "正在校验登录…");
        AuthApi::getInstance()->checkSession();
    } else {
        updateLoginUi(loginStatus, loginButton);
    }
}

void MenuUI::showMenu() {
    MainUI::getInstance()->hide();

    // 返回菜单时确保联机会话被释放（关闭对局窗口也会走到这里）。
    if (OnlineSession::getInstance()->isActive()) {
        OnlineSession::getInstance()->stop();
        WsClient::getInstance()->disconnectNow();
    }
    if (AuthStore::isLoggedIn()) {
        updateLoginUi(loginStatus, loginButton, "正在校验登录…");
        AuthApi::getInstance()->checkSession();
    } else {
        updateLoginUi(loginStatus, loginButton);
    }
    show();
}
