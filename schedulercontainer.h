#ifndef SCHEDULERCONTAINER_H
#define SCHEDULERCONTAINER_H

#include <QObject>
#include "scheduler.h"

class SchedulerContainer : public QObject
{
    Q_OBJECT
public:
    explicit SchedulerContainer(QObject *parent = 0);
    Scheduler* getSchedule(int);
    void save();
    void loadSchedules();


  private:
    Scheduler* _container[5];


signals:

public slots:
};

#endif // SCHEDULERCONTAINER_H
