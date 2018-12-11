#include "networkmanager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent)
{

    manager= new QNetworkAccessManager();
    connect(manager,SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}


 //(function to load the webpage and receive timeseries data)
void NetworkManager::loadWebPage(){
    QNetworkRequest request;

    QString apikey = "2HJ6DUVQ3B4I2OQS";

    QString urlString = QString("https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=MSFT&apikey=demo").arg(apikey);

    QUrl url(urlString);
    request.setUrl(url);

    QNetworkReply *reply = manager->get(request);
}

//Function to deserialize JSON and obtain the key value pairs of the data of interest
//need to understand it better
void NetworkManager::replyFinished(QNetworkReply *reply)
{
    QByteArray webData = reply->readAll();

    // Store requested data in a file
    QFile *file = new QFile(QDir::currentPath() + "\\DATA.txt");
    if(file->open(QFile::Append))
    {
      file->write(webData);
      file->flush();
      file->close();
    }
    delete file;

    QList<QPair<QString,QString>> graphValues1;
    QList<QPair<QString,QString>> graphValuesVolume;
    QList<QPair<QString,QString>> graphValues;


    QJsonDocument doc = QJsonDocument::fromJson(webData);

    if(doc.isArray()==true){
        QJsonArray rootArray = doc.array();
        //retrieve array
    }

    else if (doc.isObject() == true){

        QJsonObject rootObject = doc.object();

        QJsonObject timeSeries = rootObject["Time Series (Daily)"].toObject();
        QStringList keys = timeSeries.keys();

        for (QString k :keys){
            QJsonObject dayValues = timeSeries[k].toObject();
           QString Value = dayValues["1. open"].toString();
           QString volume = dayValues["5. volume"].toString();
           QString Value1 = dayValues["3. low"].toString();

            QPair<QString,QString> dataItem;
            dataItem.first = k;
            dataItem.second = Value;

            QPair<QString,QString> dataItem2;
            dataItem2.first = k;
            dataItem2.second = volume;

           QPair<QString,QString> dataItem3;
            dataItem3.first = k;
            dataItem3.second = Value1;



            graphValues1.append(dataItem);
            graphValuesVolume.append(dataItem2);
            graphValues.append(dataItem3);

            }

    }
    for (int i=0; i<graphValues1.size(); i++){
        QPair<QString,QString> data = graphValues1[i];
        float list=data.second.toFloat();
        QDateTime xAxisValue; xAxisValue.setDate(QDate::fromString(data.first,"yyyy-MM-dd"));
        xAxisValue.toMSecsSinceEpoch();
        emit valueUpdated(QVariant(xAxisValue),QVariant(list));
    }

    for (int i=0; i<graphValuesVolume.size(); i++){
        QPair<QString,QString> data = graphValuesVolume[i];
        float list = data.second.toFloat();
        QDateTime xAxisValue; xAxisValue.setDate(QDate::fromString(data.first,"yyyy-MM-dd"));
        xAxisValue.toMSecsSinceEpoch();
        emit valueUpdated2(QVariant(xAxisValue),QVariant(list));
    }

    for (int i=0; i<graphValues.size(); i++){
        QPair<QString,QString> data = graphValues[i];
        float list=data.second.toFloat();
        QDateTime xAxisValue; xAxisValue.setDate(QDate::fromString(data.first,"yyyy-MM-dd"));
        xAxisValue.toMSecsSinceEpoch();
        emit valueUpdated3(QVariant(xAxisValue),QVariant(list));
    }

}

