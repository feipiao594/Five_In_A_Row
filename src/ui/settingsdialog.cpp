#include "settingsdialog.h"

#include "../model/manager.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent),
      mainLayout(new QVBoxLayout),
      title(new QLabel("设置")),
      aiDelayLabel(new QLabel("电脑落子延迟 (ms)")),
      aiDelaySpin(new QSpinBox),
      buttonRow(new QHBoxLayout),
      closeButton(new QPushButton("关闭")) {
  setWindowTitle("设置");
  setModal(true);
  setFixedSize(QSize(360, 180));
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

  buttonRow->addStretch(1);
  closeButton->setMinimumSize(QSize(96, 34));
  closeButton->setStyleSheet("font-size: 14px;");
  buttonRow->addWidget(closeButton);
  mainLayout->addLayout(buttonRow);

  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(aiDelaySpin, &QSpinBox::valueChanged, Manager::getInstance(),
          &Manager::setAiDelayMs);
}

