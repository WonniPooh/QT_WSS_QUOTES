#include "jsonhandler.h"
#include <QByteArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

jsonHandler::jsonHandler(QObject *parent) : QObject(parent)
{
}

void jsonHandler::handle_json(QString &input)
{
    // Parse document
    QJsonDocument doc(QJsonDocument::fromJson(input.toLatin1()));

    // Get JSON object
    QJsonObject json = doc.object();

    if(json["time"] == QJsonValue::Undefined)
    {
        time = 0;
        close_value = 1;
        return;
    }

    time = json["time"].toDouble();
    close_value = json["close"].toDouble();
}

double jsonHandler::get_value()
{
    return close_value;
}

double jsonHandler::get_time()
{
    return time;
}
