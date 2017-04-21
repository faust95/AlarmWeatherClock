#include "snoozer.h"
#include "ui_snoozer.h"
#include "alarm.h"
#include <QTime>
#include <QTimer>

Snoozer::Snoozer(QWidget *parent, Alarm *currentAlarm) :
    QDialog(parent),
    ui(new Ui::Snoozer)
{
    ui->setupUi(this);
    this->_currentAlarm=currentAlarm;
    this->_snoozerTimer= new QTimer(this);
    this->isTurnedOff=false;
    setupClock();

    // set up connections
    connect(ui->snoozeButton, SIGNAL(clicked()), SLOT(snoozeClicked()));
    connect(ui->turnOffButton, SIGNAL(clicked()),SLOT(turnOffClicked()));
    connect(this->_snoozerTimer, SIGNAL(timeout()), SLOT(timerOut()));
}

Snoozer::~Snoozer()
{
    delete ui;
}

void Snoozer::setupClock()
{
    // setup of clock display
    updateClock();
    QTimer *currentTime=new QTimer(this);
    connect(currentTime,SIGNAL(timeout()),this,SLOT(updateClock()));
    currentTime->start(500);
}

void Snoozer::updateClock()
{
    ui->alarmLabel->setText(QTime::currentTime().toString("h:mm:ss ap"));
}

void Snoozer::snoozeClicked()
{
    this->_snoozerTimer->start(300000);
    this->_currentAlarm->stopAlarm();
}

void Snoozer::turnOffClicked()
{
    this->_snoozerTimer->stop();
    this->_currentAlarm->stopAlarm();
    this->hide();
    this->~Snoozer();
}

void Snoozer::timerOut()
{
    this->_currentAlarm->startAlarm(this->_currentAlarm->isCustomPathUsed);
    this->_currentAlarm->canResume=false;
}


