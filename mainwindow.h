#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "timer.h"
#include "weatherdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Timer *timerKeep;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    Alarm *currAlarm;
    SchedulerContainer *_schedules;
    WeatherModel *currentWeatherModel;
    WeatherData *currentWeather;

    int _currentAlarm;
    bool _isTwentyHourTime;
    bool _previousTimeWasTwentyHour;
    bool _isAmPmWarningEnabled;

    void displayTimeMode();
    void closeEvent(QCloseEvent*);
    void showActiveAlarm(Scheduler*);
    void setupClock();
    void setupWeather();
    void updateClock();
    void updateWeather();



private slots:

   // void setCESTTime();
    void setWeekDayTime();
    void setWeekEndTime();
    void setCustomTime();
    void toggleWeekDay(bool);
    void toggleWeekEnd(bool);
    void toggleCustomTime(bool);
   // void setAlarmSound();
    void setAlarmNumber();
    void showAbout();
    void showSettings();
    void snoozeMenuCheck();
    void timeCheck();
    void weatherCheck();
    void showWindow();
    void showWindow(QSystemTrayIcon::ActivationReason);
    void testAlarm();
    void openDiaglog(bool);


    void quit();



};

#endif // MAINWINDOW_H
