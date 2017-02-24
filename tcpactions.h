#ifndef TCPACTIONS_H
#define TCPACTIONS_H

#include <QObject>
#include <QPlainTextEdit>
#include "tcpclient.h"

class TcpActions : public QObject
{
    Q_OBJECT
public:
    explicit TcpActions(QPlainTextEdit *log, QObject *parent = 0);

public slots:
    void slotNewMessage(int asset_id, long time, double data);

private:
    QPlainTextEdit *tcp_log;
    TcpClient client;
};

#endif // TCPACTIONS_H
