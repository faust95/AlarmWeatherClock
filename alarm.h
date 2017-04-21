#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QMediaPlayer>
#include "weatherdata.h"

class Alarm : public QObject
{
    Q_OBJECT
public:
    explicit Alarm(QObject *parent = 0);
    void startAlarm(bool);
    void stopAlarm();
    void setUserPath(QString path);
    void setDefaultPath(QString path);
    bool isAlarmPlaying();
    bool isCustomPathUsed;
    bool canResume;

private:
    bool _isPlaying;


    QMediaPlayer *media;
    QString _defaultPath;
    QString _userPath;
    WeatherData *currentWeather;
    WeatherModel *currentWeatherModel;

    QTimer *_pause;


signals:

public slots:
    void setAlarmVolume(int volume);
    void resume();
    void repeatAlarm(QMediaPlayer::MediaStatus state);

};

#endif // ALARM_H
