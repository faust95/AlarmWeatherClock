#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QDateTime>
#include "alarm.h"
#include "scheduler.h"
#include "schedulercontainer.h"

class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = 0, SchedulerContainer* coll=0);
    void startTimer(Alarm*);

private:

    void setUserSound(int);

    Scheduler* _schedules[5];
    Alarm* _currentAlarm;


signals:

private slots:
  void checkAlarm();
};

#endif // TIMER_H
