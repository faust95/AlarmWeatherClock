#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include "scheduler.h"
#include "schedulercontainer.h"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    static bool extractAudio();
    static bool deleteExtractedAudio();
    Scheduler* loadConfig(int);
    bool save(SchedulerContainer*);
    static int loadVolume();
    static void saveVolume(int);
    static bool loadWindowShow();
    static void saveWindowShow(bool);
    static bool isTwentyHourTime();
    static void saveTimeMode(bool isTwentyHourTime);
    static bool loadWarnOnAmPm();
    static void saveWarnOnAmPm(bool);





private:
    QSettings _settings;

signals:

public slots:
};

#endif // SETTINGS_H
