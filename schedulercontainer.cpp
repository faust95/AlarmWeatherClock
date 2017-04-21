#include "schedulercontainer.h"
#include "scheduler.h"
#include "settings.h"
#include <QString>
#include <QSettings>

SchedulerContainer::SchedulerContainer(QObject *parent) : QObject(parent)
{
    // Initialization of our schedulers
    for(int i = 0; i < 5; i++)
    {
        this->_container[i]=new Scheduler(this);
    }
}

void SchedulerContainer::loadSchedules()
{
     Settings *settings=new Settings(this);
     for (int i = 0; i < 5; i++)
     {
         this->_container[i]=settings->loadConfig(i);
     }
}

Scheduler* SchedulerContainer::getSchedule(int index)
{
    return this->_container[index];
}

void SchedulerContainer::save()
{
    Settings *settings= new Settings(this);
    settings->save(this);
}
