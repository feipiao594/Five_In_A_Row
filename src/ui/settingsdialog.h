#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private:
    QVBoxLayout* mainLayout;
    QLabel* title;

    QLabel* aiDelayLabel;
    QSpinBox* aiDelaySpin;

    QLabel* netTitle;
    QLabel* serverLabel;
    QLineEdit* serverEdit;
    QCheckBox* ignoreSslErrors;
    QLabel* loginStatus;
    QPushButton* logoutButton;

    QHBoxLayout* buttonRow;
    QPushButton* closeButton;
};

#endif // SETTINGSDIALOG_H
