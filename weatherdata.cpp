#include "weatherdata.h"
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QSignalMapper>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QLoggingCategory>
#include <QNetworkSession>
#include <QNetworkConfigurationManager>
//#include <QStringLiteral>

/*
 *We are using weather information from http://openweathermap.org/api
 **/
#define ZERO_KELVIN 273.15

Q_LOGGING_CATEGORY(requestsLog,"wapp.requests")

WeatherData::WeatherData(QObject *parent) : QObject(parent)
{
}

WeatherData::WeatherData(const WeatherData &other) :
    QObject(0),
    m_dayOfWeek(other.m_dayOfWeek),
    m_weather(other.m_weather),
    m_weatherDescription(other.m_weatherDescription),
    m_temperature(other.m_temperature)
{
}

QString WeatherData::dayOfWeek() const
{
    return m_dayOfWeek;
}

/*!
 * The icon value is based on OpenWeatherMap.org icon set. For details
 * see http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
 *
 * e.g. 01d ->sunny day
 *
 * The icon string will be translated to
 * http://openweathermap.org/img/w/01d.png
 */

QString WeatherData::weatherIcon() const
{
    return m_weather;
}

QString WeatherData::weatherDescription() const
{
    return m_weatherDescription;
}

QString WeatherData::temperature() const
{
    return m_temperature;
}

void WeatherData::setDayOfWeek(const QString &value)
{
    m_dayOfWeek = value;
    emit dataChanged();
}

void WeatherData::setWeatherIcon(const QString &value)
{
    m_weather = value;
    emit dataChanged();
}

void WeatherData::setWeatherDescription(const QString &value)
{
    m_weatherDescription = value;
    emit dataChanged();
}

void WeatherData::setTemperature(const QString &value)
{
    m_temperature = value;
    emit dataChanged();
}

class WeatherModelPrivate
{

public:
    static const int baseMsBeforeNewRequest = 5 * 1000; // 5 s, increased after each missing answer up to 10x
    QGeoPositionInfoSource *src;
    QGeoCoordinate coord;
    QString longtitude, latitude;
    QString city;
    QNetworkAccessManager *nam;
    QNetworkSession *ns;
    WeatherData now;
    QList<WeatherData*> forecast;
    QQmlListProperty<WeatherData> *forecastProp;
    QSignalMapper *geoReplyMapper;
    QSignalMapper *weatherReplyMapper, *forecastReplyMapper;
    bool ready;
    bool useGps;
    QElapsedTimer throttle;
    int nErrors;
    int minMsBeforeNewRequest;
    QTimer delayedCityRequestTimer;
    QTimer requestNewWeatherTimer;
    QString app_apiKey;

    WeatherModelPrivate() :
        src(NULL),
        nam(NULL),
        ns(NULL),
        forecastProp(NULL),
        ready(false),
        useGps(true),
        nErrors(0),
        minMsBeforeNewRequest(baseMsBeforeNewRequest)
    {
        delayedCityRequestTimer.setSingleShot(true);
        delayedCityRequestTimer.setInterval(1000); // 1s
        requestNewWeatherTimer.setSingleShot(false);
        requestNewWeatherTimer.setInterval(20*60*100); // 20 min
        throttle.invalidate();
        app_apiKey = "0bc8231cf8c39af311b515d3ffa7695f";


    }

};

static void forecastAppend(QQmlListProperty<WeatherData> *prop, WeatherData *val)
{
    Q_UNUSED(val);
    Q_UNUSED(prop);
}


static WeatherData *forecastAt(QQmlListProperty<WeatherData> *prop, int index)
{
    WeatherModelPrivate *d = static_cast<WeatherModelPrivate*>(prop->data);
    return d->forecast.at(index);
}

static int forecastCount(QQmlListProperty<WeatherData> * prop)
{
    WeatherModelPrivate *d = static_cast<WeatherModelPrivate*>(prop->data);
    return d->forecast.size();
}

static void forecastClear(QQmlListProperty<WeatherData> *prop)
{
    static_cast<WeatherModelPrivate*>(prop->data)->forecast.clear();
}

WeatherModel::WeatherModel(QObject *parent) :
     QObject(parent),
     d(new WeatherModelPrivate)
{
    d->forecastProp = new QQmlListProperty<WeatherData>(this, d,
                                                        forecastAppend,
                                                        forecastCount,
                                                        forecastAt,
                                                        forecastClear);

    d->geoReplyMapper = new QSignalMapper(this);
    d->weatherReplyMapper = new QSignalMapper(this);
    d->forecastReplyMapper = new QSignalMapper(this);

    connect(d->geoReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleGeoNetworkData(QObject*)));
    connect(d->weatherReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleWeatherNetworkData(QObject*)));
    connect(d->forecastReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleForecastNetworkData(QObject*)));
    connect(&d->delayedCityRequestTimer, SIGNAL(timeout()),
            this, SLOT(queryCity()));
    connect(&d->requestNewWeatherTimer, SIGNAL(timeout()),
            this, SLOT(refreshWeather()));
    d->requestNewWeatherTimer.start();

    // reasurring of an active network session
    d->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm;
    d->ns = new QNetworkSession(ncm.defaultConfiguration(), this);
    connect(d->ns, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    // network session may be already open. if it is , run the slot directly
    if(d->ns->isOpen())
         this->networkSessionOpened();
    // tell the system we want network
    d->ns->open();


}

WeatherModel::~WeatherModel()
{
  d->ns->close();
  if (d->src)
       d->src->stopUpdates();
  delete d;
}

void WeatherModel::networkSessionOpened()
{
   d->src = QGeoPositionInfoSource::createDefaultSource(this);

   if(d->src)
   {
      d->useGps = true;
      connect(d->src, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
      connect(d->src,SIGNAL(error(QGeoPositionInfoSource::Error)),
                this, SLOT(positionError(QGeoPositionInfoSource::Error)));
      d->src->startUpdates();
    } else {
    d->useGps = false;
    d->city = "Wroclaw";
   // d->now.setWeatherDescription("clear sky");
  //  d->now.setTemperature("24 Celcius");
 //   d->now.setWeatherIcon("01d");
    emit cityChanged();
    this->refreshWeather();

  }

}

void WeatherModel::positionUpdated(QGeoPositionInfo gpsPos)
{
 d->coord = gpsPos.coordinate();

 if (!(d->useGps))
       return;
  queryCity();
}

void WeatherModel::queryCity()
{
 //don't update more often then once a minute
    //to keep load on server low
    if(d->throttle.isValid() && d->throttle.elapsed() < d->minMsBeforeNewRequest) {
       qCDebug(requestsLog) << "delaying query of city";
       if (!d->delayedCityRequestTimer.isActive())
             d->delayedCityRequestTimer.start();
           return;
       }

     qDebug(requestsLog) << "requested query of city";
     d->throttle.start();
     d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;

      QString latitude , longtitude;
      latitude.setNum(d->coord.longitude());
      longtitude.setNum(d->coord.latitude());

      QUrl url("http://api.openweathermap.org/data/2.5/weather");
      QUrlQuery query;
      query.addQueryItem("lat", latitude);
      query.addQueryItem("lon", longtitude);
      query.addQueryItem("mode", "json");
      query.addQueryItem("APPID", d->app_apiKey);
      url.setQuery(query);
      qCDebug(requestsLog) << "submitting request";

      QNetworkReply *rep = d->nam->get(QNetworkRequest(url));

      d->geoReplyMapper->setMapping(rep, rep);
      connect(rep, SIGNAL(finished()),
              d->geoReplyMapper, SLOT(map()));
}

void WeatherModel::positionError(QGeoPositionInfoSource::Error e)
{
    Q_UNUSED(e);
    qWarning() << "Position source error. Falling back to simulation mode";
    // cleanup
    d->src->stopUpdates();
    d->src->deleteLater();
    d->src = 0;

    // activate simulation mode
    d->useGps = false;
    d->city = "Wroclaw";
    WeatherData* currentWeather = getWeather();
    d->now.setWeatherDescription(currentWeather->weatherDescription());
    d->now.setTemperature(currentWeather->temperature());
    d->now.setWeatherIcon(currentWeather->weatherIcon());
    emit cityChanged();
    this->refreshWeather();
}


void WeatherModel::hadError(bool tryAgain)
{
  qCDebug(requestsLog) << "hadError, will " << (tryAgain ? "" : "not ") << "rety";
  d->throttle.start();
  if (d->nErrors < 10)
      ++d->nErrors;
  d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
  if (tryAgain)
      d->delayedCityRequestTimer.start();

}

/*
void WeatherModel::getCity()
{
    qCDebug(requestsLog) << "refreshing weather";
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("q", d->city);
    query.addQueryItem("mode","json");
    query.addQueryItem("APPID",d->app_apiKey);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));

    handleWeatherNetworkData(&rep);

    WeatherData *currentWeather = new WeatherData(&d->now);


}
*/

void WeatherModel::handleGeoNetworkData(QObject *replyObj)
{
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if(!networkReply) {
        hadError(false); // retry or not
        return;
    }

    if(!networkReply->error())
    {
        d->nErrors = 0;
        if (!d->throttle.isValid())
            d->throttle.start();
        d->minMsBeforeNewRequest = d->baseMsBeforeNewRequest;
        // convert coord to city name
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        QJsonObject jo = document.object();
        QJsonValue jv = jo.value("name");

        const QString city = jv.toString();
        qCDebug(requestsLog) << "got city: " << city;
        if (city != d->city) {
            d->city = city;
            emit cityChanged();
            refreshWeather();
        }
    } else {
        hadError(true);
    }
    networkReply->deleteLater();

}

void WeatherModel::refreshWeather()
{
    if (d->city.isEmpty()) {
        qCDebug(requestsLog) << "refreshing weather skippped (no city)";
        return;
    }

    qCDebug(requestsLog) << "refreshing weather";
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("q", d->city);
    query.addQueryItem("mode","json");
    query.addQueryItem("APPID",d->app_apiKey);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right way
    d->weatherReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()),
            d->weatherReplyMapper, SLOT(map()));

}

/*
WeatherData* WeatherModel::getWeather() const
{


    qCDebug(requestsLog) << "refreshing weather";
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("q", d->city);
    query.addQueryItem("mode","json");
    query.addQueryItem("APPID",d->app_apiKey);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));

    handleWeatherNetworkData(&rep);

    WeatherData *currentWeather = new WeatherData(&d->now);

    return currentWeather;



}
*/

static QString tempString(double t)
{
    return QString::number(qRound(t - ZERO_KELVIN)) + QChar(0xB0);
}

void WeatherModel::handleWeatherNetworkData(QObject *replyObj)
{
    qCDebug(requestsLog) << "got weather network data";
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if(!networkReply)
        return;

    if(!networkReply->error()) {
        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();

        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue val;

            if (obj.contains("weather")) {
                val = obj.value("weather");
                QJsonArray weatherArray = val.toArray();
                val = weatherArray.at(0);
                tempObject = val.toObject();
                d->now.setWeatherDescription(tempObject.value("description").toString());
                d->now.setWeatherIcon(tempObject.value("icon").toString());
            }

            if(obj.contains("main")) {
                val = obj.value("main");
                tempObject = val.toObject();
                val = tempObject.value("temp");
                d->now.setTemperature(tempString(val.toDouble()));
            }
        }
    }

    networkReply->deleteLater();

    // retrieve weather forecast
    QUrl url("http://api.openweathermap.org/data/2.5/forecast/daily");
    QUrlQuery query;

    query.addQueryItem("q", d->city);
    query.addQueryItem("mode", "json");
    query.addQueryItem("cnt", "5");
    query.addQueryItem("APPID", d->app_apiKey);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up signal
    d->forecastReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()), d->forecastReplyMapper, SLOT(map()));

}

WeatherData* WeatherModel::getWeather()
{
    QUrl url("http://api.openweathermap.org/data/2.5/forecast/daily");
    QUrlQuery query;

    query.addQueryItem("q", "Wroclaw");
    query.addQueryItem("mode", "json");
    query.addQueryItem("cnt", "5");
    query.addQueryItem("APPID", d->app_apiKey);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // retrieve weather forecast

    if(!rep->error()) {
        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();


        QJsonDocument document = QJsonDocument::fromJson(rep->readAll());

        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue val;

            if (obj.contains("weather")) {
                val = obj.value("weather");
                QJsonArray weatherArray = val.toArray();
                val = weatherArray.at(0);
                tempObject = val.toObject();
                d->now.setWeatherDescription(tempObject.value("description").toString());
                d->now.setWeatherIcon(tempObject.value("icon").toString());
            }

            if(obj.contains("main")) {
                val = obj.value("main");
                tempObject = val.toObject();
                val = tempObject.value("temp");
                d->now.setTemperature(tempString(val.toDouble()));
            }
        }
    }

    WeatherData * weatherInfo = new WeatherData(d->now);



   rep->deleteLater();


   return weatherInfo;



}


void WeatherModel::handleForecastNetworkData(QObject *replyObj)
{
    qCDebug(requestsLog) << "got forecast";
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if (!networkReply)
       return;

    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        QJsonObject jo;
        QJsonValue jv;
        QJsonObject root = document.object();
        jv = root.value("list");
        if (!jv.isArray())
            qWarning() << "Invalid forecast object";
        QJsonArray ja = jv.toArray();
        // we need another days of forecast --> first entry is today
        if(ja.count() != 5)
            qWarning() << "Invalid forecast object";

        QString data;

        for (int i = 1; i < ja.count(); i++) {
            WeatherData *forecastEntry = new WeatherData();

            // min/max temperature
            QJsonObject subtree = ja.at(i).toObject();
            jo = subtree.value("temp").toObject();
            jv = jo.value("min");
            data.clear();
            data += tempString(jv.toDouble());
            data += QChar('/');
            jv = jo.value("max");
            data += tempString(jv.toDouble());
            forecastEntry->setTemperature(data);

            // get date
            jv = subtree.value("dt");
            QDateTime dt = QDateTime::fromMSecsSinceEpoch((qint64)jv.toDouble()*1000);
            forecastEntry->setDayOfWeek(dt.date().toString("ddd"));

            // get icon
            QJsonArray weatherArray = subtree.value("weather").toArray();
            jo = weatherArray.at(0).toObject();
            forecastEntry->setWeatherIcon(jo.value("icon").toString());

            //get description
            forecastEntry->setWeatherDescription(jo.value("description").toString());

            d->forecast.append(forecastEntry);
        }

        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }

        emit weatherChanged();

    }
    networkReply->deleteLater();
}

bool WeatherModel::hasValidCity() const
{
    return (!(d->city.isEmpty()) && d->city.size() > 1 && d->city != "");
}

bool WeatherModel::hasValidWeather() const
{
    return hasValidCity() && (!(d->now.weatherIcon().isEmpty()) &&
                               (d->now.weatherIcon().size() > 1) &&
                               d->now.weatherIcon() != "");
}

WeatherData* WeatherModel::weather() const
{
  return &(d->now);
}

QQmlListProperty<WeatherData> WeatherModel::forecast() const
{
    return *(d->forecastProp);
}

bool WeatherModel::ready() const
{
    return d->ready;
}

bool WeatherModel::hasSource() const
{
    return (d->src != NULL);
}

bool WeatherModel::useGps() const
{
    return d->useGps;
}

void WeatherModel::setUseGps(bool value)
{
    d->useGps = value;
    if (value) {
        d->city = "";
        d->throttle.invalidate();
        emit cityChanged();
        emit weatherChanged();
    }
    emit useGpsChanged();
}

QString WeatherModel::city() const
{
    return d->city;
}

void WeatherModel::setCity(const QString &value)
{
    d->city = value;
    emit cityChanged();
    refreshWeather();
}













