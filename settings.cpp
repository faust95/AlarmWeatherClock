#include "settings.h"
#include "schedulercontainer.h"
#include "scheduler.h"
#include "weatherdata.h"
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QDateTime>
#include <QTime>


Settings::Settings(QObject *parent) : QObject(parent)
{
}

bool Settings::extractAudio()
{

    if(QFile::copy(":/new/sounds/sunny_clouds.mp3", QDir::tempPath()+ "/QtAlarm.mp3"))
    {
        return true;
    }
    return false;
}

bool Settings::deleteExtractedAudio()
{
    return QFile::remove(QDir::tempPath()+"/QtAlarm.mp3");
}

Scheduler* Settings::loadConfig(int index)
{
  Scheduler *scheduler= new Scheduler(this);
  QString idx;
  idx.setNum(index);
  bool weekDayEnabled=this->_settings.value(idx+"WeekDayEnabled").toBool();
  QTime weekDayTime=this->_settings.value(idx+"WeekDayTime").toTime();
  bool weekEndEnabled=this->_settings.value(idx+"WeekEndEnabled").toBool();
  QTime weekEndTime=this->_settings.value(idx+"WeekEndTime").toTime();
  bool  customTimeDateEnabled=this->_settings.value(idx+"CustomEnabled").toBool();
  QDateTime customTime=this->_settings.value(idx+"CustomTime").toDateTime();
  bool userSoundEnabled=this->_settings.value(idx+"userSoundEnabled").toBool();
  QString userSound=this->_settings.value(idx+"userSound").toString();

  if(weekDayTime.isNull())
  {
      weekDayTime.setHMS(0,0,0,0);
  }
  if(weekEndTime.isNull())
  {
      weekEndTime.setHMS(0,0,0,0);
  }
  if(customTime.isNull())
  {
      customTime.time().setHMS(0,0,0,0);
      customTime.setDate(QDateTime::currentDateTime().date());
  }
  if(userSoundEnabled==false)
  {
      userSound="";
  }

  scheduler->setSchedule(customTimeDateEnabled,customTime,weekDayEnabled,weekDayTime,weekEndEnabled,weekEndTime,userSoundEnabled,userSound);
  return scheduler;
}

bool Settings::save(SchedulerContainer *coll)
{
    try
    {
        for(int i=0; i < 5; i++)
        {
            Scheduler *scheduler=coll->getSchedule(i);
            QString idx;
            idx.setNum(i);
            this->_settings.setValue(idx+"WeekDayEnabled",scheduler->getWeekDayEnabled());
            this->_settings.setValue(idx+"WeekDayTime",scheduler->getWeekDay());
            this->_settings.setValue(idx+"WeekEndEnabled", scheduler->getWeekDayEnabled());
            this->_settings.setValue(idx+"WeekEndTime",scheduler->getWeekEnd());
            this->_settings.setValue(idx+"CustomEnabled",scheduler->getUserEnabled());
            this->_settings.setValue(idx+"CustomTime",scheduler->getUserSoundEnabled());
            this->_settings.setValue(idx+"UserSound",scheduler->getUserSound());
            this->_settings.sync();
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

int Settings::loadVolume()
{
    QSettings settings;
    return settings.value("Volume").toInt();
}

void Settings::saveVolume(int vol)
{
    QSettings settings;
    settings.setValue("Volume",vol);
}

bool Settings::loadWindowShow()
{
     QSettings settings;
     return settings.value("showWindowDefault").toBool();
}

void Settings::saveWindowShow(bool showWindow)
{
     QSettings settings;
     settings.setValue("showWindowDefault",showWindow);
}

bool Settings::isTwentyHourTime()
{
     QSettings settings;
     return settings.value("isTwentyHourTime").toBool();
}

void Settings::saveTimeMode(bool isTwentyHourTime)
{
    QSettings settings;
    settings.setValue("isTwentyHourTime",isTwentyHourTime);
}

bool Settings::loadWarnOnAmPm()
{
    QSettings settings;
    return settings.value("WarnOnAmPm").toBool();
}

void Settings::saveWarnOnAmPm(bool warning)
{
    QSettings settings;
    settings.value("WarnOnAmPm",warning);
}






