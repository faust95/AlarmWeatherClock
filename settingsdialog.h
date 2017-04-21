#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0,bool isTwentyHourTime=true);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    bool _isTwentyHoursTime;
    bool _warnOnAmPm;

private slots:
    void toggleWarnBox();
    void save();
};

#endif // SETTINGSDIALOG_H
