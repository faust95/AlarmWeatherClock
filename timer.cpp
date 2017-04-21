#include <QTimer>
#include <QDateTime>
#include <QDate>
#include "timer.h"
#include "scheduler.h"
#include "schedulercontainer.h"

Timer::Timer(QObject *parent, SchedulerContainer *coll) : QObject(parent)
{
   for(int i = 0; i < 5;i++)
   {
       this->_schedules[i]=coll->getSchedule(i);
       if(this->_schedules[i]==NULL)
       {
           this->_schedules[i]=new Scheduler(this);
       }
   }
}


void Timer::startTimer(Alarm *mainAlarm)
{
    this->_currentAlarm=mainAlarm;
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(checkAlarm()));
    timer->start(1000);
}


void Timer::setUserSound(int i)
{
    if(this->_schedules[i]->getUserSoundEnabled())
    {
        this->_currentAlarm->setUserPath(this->_schedules[i]->getUserSound());
    }
}

void Timer::checkAlarm()
{
    // Comparing saved times with current Time
    if(!this->_currentAlarm->isAlarmPlaying() && this->_currentAlarm->canResume)
    {
        for(int i = 0; i < 5; i++)
        {
            QDateTime currentTime=QDateTime::currentDateTime();
            switch(currentTime.date().dayOfWeek())
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                // Day of the Week Alarms
                if(this->_schedules[i]->getWeekDayEnabled() && this->_schedules[i]->getWeekDay().hour()==currentTime.time().hour() &&
                        this->_schedules[i]->getWeekDay().minute()==currentTime.time().minute())
                {
                    setUserSound(i);
                    this->_currentAlarm->startAlarm(this->_schedules[i]->getUserSoundEnabled());
                }
                break;
            default:
                // Day of WeekEnd Alarms
            if(this->_schedules[i]->getWeekEndEnabled() && this->_schedules[i]->getWeekEnd().hour()==currentTime.time().hour() &&
                    this->_schedules[i]->getWeekEnd().minute()==currentTime.time().minute())
            {
                setUserSound(i);
                this->_currentAlarm->startAlarm(this->_schedules[i]->getUserSoundEnabled());
            }
           }

            // Check for User Date Alarm
            if(this->_schedules[i]->getUserEnabled() && this->_schedules[i]->getUserDate().date()==currentTime.date() &&
                    this->_schedules[i]->getUserDate().time().minute()==currentTime.time().minute()
                    && this->_schedules[i]->getUserDate().time().hour()==currentTime.time().hour())
            {
                setUserSound(i);
                this->_currentAlarm->startAlarm(this->_schedules[i]->getUserSoundEnabled());
            }



            }
        }
}


