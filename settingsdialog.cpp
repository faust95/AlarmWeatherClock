#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent, bool isTwentyHourTime) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    _isTwentyHoursTime=isTwentyHourTime;

    if (_isTwentyHoursTime)
    {
        ui->AmPmTimeWarningcheckBox->setDisabled(true);
    }



    ui->AmPmTimeWarningcheckBox->setChecked(_warnOnAmPm);

    ui->amPMButton->setChecked(!isTwentyHourTime);
    ui->twentyHourButton->setChecked(isTwentyHourTime);

    ui->showWindowcheckBox->setChecked(Settings::loadWindowShow());

    connect(ui->saveSettingsButton,SIGNAL(clicked(bool)),this, SLOT(save()));
    connect(ui->cancelSettingsButton,SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->amPMButton,SIGNAL(clicked(bool)), this, SLOT(toggleWarnBox()));
    connect(ui->twentyHourButton,SIGNAL(clicked(bool)),this,SLOT(toggleWarnBox()));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::save()
{
    Settings::saveWindowShow(ui->showWindowcheckBox->isChecked());
    _isTwentyHoursTime=ui->twentyHourButton->isChecked();
    _warnOnAmPm=ui->AmPmTimeWarningcheckBox->isChecked();
    Settings::saveTimeMode(_isTwentyHoursTime);
    Settings::saveWarnOnAmPm(_warnOnAmPm);
    this->deleteLater();

}

void SettingsDialog::toggleWarnBox()
{
    if(ui->amPMButton->isChecked())
    {
        ui->AmPmTimeWarningcheckBox->setDisabled(false);
    } else {
        ui->AmPmTimeWarningcheckBox->setDisabled(true);
    }

}
