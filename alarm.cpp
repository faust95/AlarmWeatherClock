#include "alarm.h"
#include "settings.h"
#include "weatherdata.h"
#include <QMediaPlayer>
#include <QString>
#include <QDir>

Alarm::Alarm(QObject *parent) : QObject(parent)
{
    media = new QMediaPlayer(this);
    this->_isPlaying=false;
    this->_defaultPath= ":/new/sounds/sunny_clouds.mp3";

    qDebug() << _defaultPath;

    this->_pause=new QTimer(this);
    this->canResume=true;
    this->currentWeatherModel = new WeatherModel(this);
    this->currentWeather = currentWeatherModel->weather();

    connect(this->_pause, SIGNAL(timeout()), this, SLOT(resume()));

}

void Alarm::startAlarm(bool isCustom)
{
    if(isCustom)
    {
        media->setMedia(QUrl::fromLocalFile(this->_userPath));
        this->isCustomPathUsed = true;
    } else
    {

       Settings::extractAudio();

     //   QFile::copy(":/new/sounds/sunny_clouds.mp3", QDir::tempPath()+ "/QtAlarm.mp3");

        qDebug() << _defaultPath;


        if(currentWeather->weatherIcon() == "01d") {
             media->setMedia(QUrl("qrc:/new/sounds/sunny_clouds.mp3"));
        }
        else if (currentWeather->weatherIcon() == "10d") {
            media->setMedia(QUrl("qrc:/new/sounds/blitz_alarm.mp3"));
        }
        else if (currentWeather->weatherIcon() == "13d") {
              media->setMedia(QUrl("qrc:/new/sounds/winter_alarm.mp3"));
        } else if (currentWeather->weatherIcon() == "02d") {
              media->setMedia(QUrl("qrc:/new/sounds/sunny_clouds.mp3"));
        } else if (currentWeather->weatherIcon() == "11d") {
             media->setMedia(QUrl("qrc:/new/sounds/storm_alarm.mp3"));
        } else if (currentWeather->weatherIcon() == "03d" || currentWeather->weatherIcon() == "04d" ) {
             media->setMedia(QUrl("qrc:/new/sounds/sunny_clouds.mp3"));
        }


       // media->setMedia(QUrl::fromLocalFile(this->_defaultPath));
        this->isCustomPathUsed = false;
    }
    media->play();
    connect(media,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(repeatAlarm(QMediaPlayer::MediaStatus)));
    this->_isPlaying=true;
}

void Alarm::stopAlarm()
{
   media->stop();
   this->_pause->start(100000);
   this->_isPlaying=false;
}

void Alarm::setAlarmVolume(int volume)
{
    media->setVolume(volume);
}

void Alarm::repeatAlarm(QMediaPlayer::MediaStatus state)
{
    // if sound file is ended
    if(state==QMediaPlayer::EndOfMedia)
    {
       if(this->isCustomPathUsed)
       {
           media->setMedia(QUrl::fromLocalFile(this->_userPath));
       }
       else
       {
          media->setMedia(QUrl::fromLocalFile(this->_defaultPath));
       }
       media->play();
    }
}

void Alarm::resume()
{
    this->canResume=true;
    this->_pause->stop();
}

void Alarm::setUserPath(QString path)
{
    this->_userPath=path;
}

void Alarm::setDefaultPath(QString path)
{
    this->_defaultPath=path;
}

bool Alarm::isAlarmPlaying()
{
    return this->_isPlaying;
}

