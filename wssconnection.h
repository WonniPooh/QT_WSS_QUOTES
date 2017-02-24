#ifndef WSSCONNECTION_H
#define WSSCONNECTION_H

#include <QObject>
#include <QWebSocket>

#include "jsonhandler.h"
#include "controlpanel.h"
#include "statisticsfilesystem.h"

class WssConnection : public QObject
{
    Q_OBJECT
public:
    explicit WssConnection(QObject *parent = 0);
    ~WssConnection();
    QVBoxLayout *getPanelLayout();
    void setConnectionConfiguration(int id, QString name, QString query); //+ Tcp client

signals:
    void newMessage(int asset_id, QString message);

public slots:
    void onConnected();
    void slotThreadStart();
    void onSslErrors(const QList<QSslError> &errors);
    void onTextMessageReceived(QString message);

private:
    const int max_msg_length = 1000;
    int     asset_id;
    QUrl    connection_url;
    QString asset_query;
    QString asset_name;

    QFile                   *file_save_to;
    QWebSocket              *m_webSocket;
    jsonHandler             *json_parser;
    ControlPanel            *m_panel;
    StatisticsFileSystem    *m_filesys;
};

#endif // WSSCONNECTION_H
