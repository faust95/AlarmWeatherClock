#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTime>
#include "scheduler.h"



class Scheduler : public QObject
{
    Q_OBJECT
public:
    explicit Scheduler(QObject *parent = 0);
    void setSchedule(bool,QDateTime,bool,QTime,bool,QTime,bool,QString);


    void setWeekDayEnabled(bool);
    void setWeekEndDayEnabled(bool);

    void setUserEnabled(bool);

    void setWeekDay(QTime);
    void setWeekEnd(QTime);
    void setUser(QDateTime);
    void setUserSound(QString);
    void setUserSoundEnabled(bool);

    bool getUserEnabled();
    bool getUserSoundEnabled();

    bool getWeekDayEnabled();
    bool getWeekEndEnabled();

    QTime getWeekDay();
    QTime getWeekEnd();
    QDateTime getUserDate();
    QString getUserSound();

private:

    QTime _weekDayAlarm;
    QTime _weekEndAlarm;
    QDateTime _userAlarm;
    QString _userSound;



    bool _userEnabled;
    bool _userSoundEnabled;
    bool _weekDayEnabled;
    bool _weekEndEnabled;



signals:

public slots:
};

#endif // SCHEDULER_H
