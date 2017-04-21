#ifndef SNOOZER_H
#define SNOOZER_H

#include <QDialog>
#include <QTimer>
#include "alarm.h"

namespace Ui {
class Snoozer;
}

class Snoozer : public QDialog
{
    Q_OBJECT

public:
    explicit Snoozer(QWidget *parent = 0, Alarm* currentAlarm = 0);
    ~Snoozer();
    bool isTurnedOff;

public slots:
    void updateClock();

private slots:
    void snoozeClicked();
    void turnOffClicked();
    void timerOut();


private:
    Ui::Snoozer *ui;
    void setupClock();
    Alarm *_currentAlarm;
    QTimer *_snoozerTimer;
};

#endif // SNOOZER_H
