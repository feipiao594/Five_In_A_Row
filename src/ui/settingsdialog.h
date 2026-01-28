#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>

class SettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QLabel *title;

  QLabel *aiDelayLabel;
  QSpinBox *aiDelaySpin;

  QHBoxLayout *buttonRow;
  QPushButton *closeButton;
};

#endif // SETTINGSDIALOG_H

