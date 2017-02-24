#include "tcpactions.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

TcpActions::TcpActions(QPlainTextEdit* log, QObject *parent) : QObject(parent)
{
    tcp_log = log;
    client.setUpConnection("localhost", 2323, 11000);
}

void TcpActions::slotNewMessage(int asset_id, long time, double data)
{
    QJsonObject data_to_send;
    data_to_send.insert(QString("time"), QJsonValue(QString::number(time)));
    data_to_send.insert(QString("close"), QJsonValue(data));

    QJsonObject msg_to_send;
    QJsonArray dest_id_array;
    dest_id_array.push_back(QJsonValue(asset_id));
    msg_to_send.insert(QString("receiver_id"), QJsonValue(dest_id_array));
    msg_to_send.insert(QString("data"), QJsonValue(data_to_send));

    QJsonDocument doc(msg_to_send);
    QString str_json(doc.toJson(QJsonDocument::Compact));
    tcp_log->appendPlainText(str_json);
    client.sendMsg(str_json.toStdString());
}
