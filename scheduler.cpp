#include "scheduler.h"
#include <QString>
#include <QDateTime>
#include <QTime>

Scheduler::Scheduler(QObject *parent) : QObject(parent)
{

    this->_userEnabled=false;
    this->_userAlarm.time().setHMS(0,0,0,0);
    this->_userSoundEnabled = false;
    this->_userSound="";
    this->_weekDayAlarm.setHMS(0,0,0,0);
    this->_weekEndAlarm.setHMS(0,0,0,0);
    this->_weekDayEnabled=false;
    this->_weekEndEnabled=false;

}

void Scheduler::setSchedule(bool userEnabled, QDateTime userTime, bool weekDayEnabled, QTime weekDayTime, bool weekEndEnabled, QTime weekEndTime,
                       bool userSoundEnabled, QString userSoundPath)
{
    this->_userSoundEnabled=userEnabled;
    this->_userAlarm=userTime;
    this->_userSoundEnabled=userSoundEnabled;
    this->_userSound=userSoundPath;
    this->_weekDayEnabled=weekDayEnabled;
    this->_weekDayAlarm=weekDayTime;
    this->_weekEndEnabled=weekEndEnabled;
    this->_weekEndAlarm=weekEndTime;
}

void Scheduler::setUserEnabled(bool isEnabled)
{
    this->_userEnabled=isEnabled;
}

void Scheduler::setUserSoundEnabled(bool isEnabled)
{
    this->_userSoundEnabled=isEnabled;
}

void Scheduler::setUser(QDateTime dateTime)
{
    this->_userAlarm=dateTime;
}

void Scheduler::setUserSound(QString soundPath)
{
    this->_userSound=soundPath;
}

bool Scheduler::getUserEnabled()
{
    return this->_userEnabled;
}

bool Scheduler::getUserSoundEnabled()
{
    return this->_userSoundEnabled;
}

QDateTime Scheduler::getUserDate()
{
    return this->_userAlarm;
}

QString Scheduler::getUserSound()
{
    return this->_userSound;
}

void Scheduler::setWeekDayEnabled(bool isEnabled)
{
    this->_weekDayEnabled=isEnabled;
}

void Scheduler::setWeekEndDayEnabled(bool isEnabled)
{
    this->_weekEndEnabled=isEnabled;
}

void Scheduler::setWeekDay(QTime time)
{
    this->_weekDayAlarm=time;
}

void Scheduler::setWeekEnd(QTime time)
{
    this->_weekEndAlarm=time;
}

bool Scheduler::getWeekDayEnabled()
{
    return this->_weekDayEnabled;
}

bool Scheduler::getWeekEndEnabled()
{
    return this->_weekEndEnabled;
}


QTime Scheduler::getWeekDay()
{
    return this->_weekDayAlarm;
}

QTime Scheduler::getWeekEnd()
{
    return this->_weekEndAlarm;
}

