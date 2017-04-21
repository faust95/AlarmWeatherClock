#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timer.h"
#include "alarm.h"
#include "schedulercontainer.h"
#include "about.h"
#include "settings.h"
#include "snoozer.h"
#include "settingsdialog.h"
#include "weatherdata.h"
#include <QMessageBox>
#include <QTimeEdit>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCloseEvent>
#include <QSlider>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(QSystemTrayIcon::isSystemTrayAvailable())
    {

        // load Schedulers
        _schedules = new SchedulerContainer(this);
        _schedules->loadSchedules();

        // load time Keepers
        timerKeep=new Timer(this, _schedules);
        //load Alarm
        currAlarm=new Alarm(this);
        // set timer with current Alarm
        timerKeep->startTimer(currAlarm);

        // load weather
        currentWeatherModel = new WeatherModel(this);
        currentWeather = currentWeatherModel->weather();

        // set up volume
        int volume = Settings::loadVolume();
        volume = volume <=0 ? 40 : volume;
        ui->volumeSlider->setValue(volume);
        currAlarm->setAlarmVolume(ui->volumeSlider->value());

        // set other member variables
        _isTwentyHourTime=Settings::isTwentyHourTime();

        qDebug() << "Is Military time" << _isTwentyHourTime;

        _previousTimeWasTwentyHour=_isTwentyHourTime;
        _isAmPmWarningEnabled=Settings::loadWarnOnAmPm();
        displayTimeMode();


        // set up tray Icon
        trayIcon= new QSystemTrayIcon(this);
        trayIconMenu = new QMenu(this);
        QAction *qaShow = new QAction("&Show",this);
        QAction *qaQuit = new QAction("&Quit",this);

        trayIconMenu->addAction(qaShow);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(qaQuit);
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->setIcon(QIcon(":/new/Icons/clock.png"));
        trayIcon->setToolTip("WeatherAlarm");
        trayIcon->show();

        ui->alarm1->setChecked(true);
        setAlarmNumber();
        setupClock();
        setupWeather();


        // setup ui slots
        connect(qaQuit,SIGNAL(triggered()), this, SLOT(quit()));
        connect(qaShow,SIGNAL(triggered()), this, SLOT(showWindow()));
        connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(showWindow(QSystemTrayIcon::ActivationReason)));
        connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(showAbout()));
        connect(ui->actionQuit, SIGNAL(triggered()),this, SLOT(quit()));
        connect(ui->actionSettings,SIGNAL(triggered(bool)), this,SLOT(showSettings()));
        connect(ui->weekDays_edit, SIGNAL(editingFinished()),this, SLOT(setWeekDayTime()));
        connect(ui->weekEndDays_edit, SIGNAL(editingFinished()), this, SLOT(setWeekEndTime()));
        connect(ui->userTimeAndDate_edit, SIGNAL(editingFinished()), this, SLOT(setCustomTime()));
        connect(ui->alarm1, SIGNAL(clicked()), this, SLOT(setAlarmNumber()));
        connect(ui->alarm2, SIGNAL(clicked()), this, SLOT(setAlarmNumber()));
        connect(ui->alarm3, SIGNAL(clicked()), this, SLOT(setAlarmNumber()));
        connect(ui->alarm4, SIGNAL(clicked()), this, SLOT(setAlarmNumber()));
        connect(ui->alarm5, SIGNAL(clicked()), this, SLOT(setAlarmNumber()));
        connect(ui->checkWeekDays, SIGNAL(clicked(bool)),this, SLOT(toggleWeekDay(bool)));
        connect(ui->checkWeekEnd, SIGNAL(clicked(bool)), this, SLOT(toggleWeekEnd(bool)));
        connect(ui->checkUserDateTime, SIGNAL(clicked(bool)), this, SLOT(toggleCustomTime(bool)));
        connect(ui->checkSound, SIGNAL(clicked(bool)), this, SLOT(openDiaglog(bool)));
        connect(ui->testButton, SIGNAL(clicked()), this, SLOT(testAlarm()));
        connect(ui->volumeSlider, SIGNAL(valueChanged(int)),currAlarm, SLOT(setAlarmVolume(int)));
        connect(ui->calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setCustomTime()));
        connect(ui->weatherButton, SIGNAL(clicked()), this, SLOT(weatherCheck()));

    }
    else
    {
        QMessageBox::critical(this,"Abort","Error of application. Exiting...");
        exit(EXIT_FAILURE);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
     event->ignore();
     this->hide();
}

void MainWindow::setupClock()
{
    // setup display of Clock Alarm
    QTimer *currentTime= new QTimer(this);
    connect(currentTime, SIGNAL(timeout()), this, SLOT(timeCheck()));
    currentTime->start(500);
}

void MainWindow::showWindow(QSystemTrayIcon::ActivationReason reason)
{
    if (reason==QSystemTrayIcon::DoubleClick || reason==QSystemTrayIcon::Trigger)
    {
        showWindow();
    }
}

void MainWindow::showWindow()
{
    if(this->currAlarm->isAlarmPlaying())
    {
        this->currAlarm->stopAlarm();
    }
    ui->testButton->setText("Test");
    this->show();
}

void MainWindow::setWeekDayTime()
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setWeekDay(ui->weekDays_edit->time());
}

void MainWindow::setWeekEndTime()
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setWeekEnd(ui->weekEndDays_edit->time());
}

void MainWindow::setCustomTime()
{
    //update date on display
    ui->userTimeAndDate_edit->setDate(ui->calendarWidget->selectedDate());
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    QDateTime userDateTime;
    userDateTime.setTime(ui->userTimeAndDate_edit->time());
    userDateTime.setDate(ui->calendarWidget->selectedDate());
    active->setUser(userDateTime);
}

void MainWindow::toggleWeekDay(bool isEnabled)
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setWeekDayEnabled(isEnabled);
}

void MainWindow::toggleWeekEnd(bool isEnabled)
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setWeekEndDayEnabled(isEnabled);
}

void MainWindow::toggleCustomTime(bool isEnabled)
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setUserEnabled(isEnabled);
}

void MainWindow::quit()
{
    this->_schedules->save();
    Settings::deleteExtractedAudio();
    Settings::saveVolume(ui->volumeSlider->value());
    qApp->quit();
}

void MainWindow::setAlarmNumber()
{
    Scheduler *activeScheduler;
    int index;

    if(ui->alarm1->isChecked())
    {
        index=0;
    }
    else if(ui->alarm2->isChecked())
    {
        index=1;
    }
    else if(ui->alarm3->isChecked())
    {
        index=2;
    }
    else if(ui->alarm4->isChecked())
    {
        index=3;
    }
    else if(ui->alarm4->isChecked())
    {
        index=4;
    }

    this->_currentAlarm=index;
    // Now display active alarm

    activeScheduler=this->_schedules->getSchedule(index);
    showActiveAlarm(activeScheduler);
}

void MainWindow::showActiveAlarm(Scheduler *active)
{
     ui->checkWeekDays->setChecked(active->getWeekDayEnabled());
     ui->weekDays_edit->setTime(active->getWeekDay());

     ui->checkWeekEnd->setChecked(active->getWeekEndEnabled());
     ui->weekEndDays_edit->setTime(active->getWeekEnd());

     ui->checkUserDateTime->setChecked(active->getUserEnabled());
     ui->userTimeAndDate_edit->setDateTime(active->getUserDate());
     ui->checkSound->setChecked(active->getUserSoundEnabled());
     ui->soundPathLabel->setText(active->getUserSound());
}

void MainWindow::timeCheck()
{
    updateClock();
    snoozeMenuCheck();

}


void MainWindow::weatherCheck()
{
    updateWeather();
}


void MainWindow::updateClock()
{
    ui->clock->setText(QTime::currentTime().toString("h:mm:ss ap"));
}

void MainWindow::updateWeather()
{
    WeatherData *currentWeather = currentWeatherModel->getWeather();

    QString weatherInfo = currentWeather->weatherDescription();

    QString iconInfo = currentWeather->weatherIcon();

    QString cityInfo = currentWeatherModel->city();

    QString tempInfo = currentWeather->temperature();

    qDebug() << "This is update debug" << weatherInfo << iconInfo << tempInfo << cityInfo;


    ui->cityInfo->setText(cityInfo);

    ui->temperatureInfo->setText(tempInfo);

    ui->weatherInfo_2->setText(weatherInfo);

    QPixmap image;

    if(iconInfo == "10d" || iconInfo =="09d" ) {
         image.load(":/new/Icons/weather-rainy.png");
    }
    else if (iconInfo == "01d") {
        image.load(":/new/Icons/weather-sunny.png");
    }
    else if (iconInfo == "13d") {
         image.load(":/new/Icons/weather-snow.png");
    } else if (iconInfo == "02d") {
         image.load(":/new/Icons/weather-few_clouds.png");
    } else if (iconInfo == "11d") {
        image.load(":/new/Icons/weather-thunderstorm.png");
    } else if (iconInfo == "03d" || iconInfo == "04d") {
        image.load(":/new/Icons/weather-few_clouds.png");
    }

    ui->weatherInfo->setPixmap(image);
}

void MainWindow::openDiaglog(bool isChecked)
{
    Scheduler *active=this->_schedules->getSchedule(this->_currentAlarm);
    active->setUserSoundEnabled(isChecked);
    if(isChecked)
    {
        QString alarmPath = QFileDialog::getOpenFileName(this,"Select Alarm Sound from File",QDir::homePath());
        active->setUserSound(alarmPath);
        ui->soundPathLabel->setText(alarmPath);
    }
    else
    {
        ui->soundPathLabel->setText("No Path specified");
    }
}

void MainWindow::testAlarm()
{
   if(ui->testButton->text()=="Test" || ui->testButton->text()=="&Test")
   {
       if(ui->checkSound->isChecked())
       {
           this->currAlarm->setUserPath(ui->soundPathLabel->text());
           this->currAlarm->startAlarm(true);
       } else {
              this->currAlarm->startAlarm(false);
       }
      ui->testButton->setText("Stop");
   } else {
       this->currAlarm->stopAlarm();
       ui->testButton->setText("Test");
   }
}

void MainWindow::showAbout()
{
    About *aboutWindow= new About(this);
    aboutWindow->show();
}

void MainWindow::snoozeMenuCheck()
{
    if(this->currAlarm->isAlarmPlaying() && this->currAlarm->canResume && ui->testButton->text()=="Alarm Test" )
    {

        qDebug() << "Snoozer working" ;
       // create snooze Menu object
       Snoozer *snoozeMenu=new Snoozer(this,currAlarm);
       snoozeMenu->show();
       this->currAlarm->canResume=false;
    }
}

void MainWindow::showSettings()
{
   SettingsDialog *settingsPage=new SettingsDialog(this);
   settingsPage->show();
}


void MainWindow::displayTimeMode()
{
    if(_isTwentyHourTime)
    {
        ui->weekDays_edit->setDisplayFormat("H:mm:ss");
        ui->weekEndDays_edit->setDisplayFormat("H:mm:ss");
        ui->userTimeAndDate_edit->setDisplayFormat("d MMM yyyy HH:mm:ss");
    }else{
        ui->weekDays_edit->setDisplayFormat("h:mm:ss ap");
        ui->weekEndDays_edit->setDisplayFormat("h:mm:ss ap");
        ui->userTimeAndDate_edit->setDisplayFormat("d MMM yyyy hh:mm:ss ap");
    }
}



void MainWindow::setupWeather()
{

    WeatherData *currentWeather = currentWeatherModel->weather();

    QString weatherInfo = currentWeather->weatherDescription();

    QString iconInfo = currentWeather->weatherIcon();

    QString tempInfo =  currentWeather->temperature();

    QString cityInfo = currentWeatherModel->city();

    qDebug() << "This is debug" << weatherInfo << iconInfo << tempInfo << cityInfo;

    ui->cityInfo->setText(cityInfo);

    ui->temperatureInfo->setText(tempInfo);

    ui->weatherInfo->setText(weatherInfo);

    weatherCheck();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(weatherCheck()));
    timer->start(5000);

    qDebug() << "This is debug update";

}


