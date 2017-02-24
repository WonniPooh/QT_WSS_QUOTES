#include "wssconnection.h"
#include <QFile>

WssConnection::WssConnection(QObject *parent) : QObject(parent)
{
    file_save_to = nullptr;
    m_filesys = nullptr;
    connection_url.setUrl("wss://olymptrade.com/ws2");
}

WssConnection::~WssConnection()
{
    delete m_filesys;
}

void WssConnection::setConnectionConfiguration(int id, QString name, QString query)
{
    asset_id = id;
    asset_name = name;
    asset_query = query;
}

void WssConnection::slotThreadStart()
{
    m_webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    m_filesys  = new StatisticsFileSystem(asset_id);
    json_parser = new jsonHandler(this);
    file_save_to = new QFile(this);

    connect(m_webSocket, &QWebSocket::connected, this, &WssConnection::onConnected);
    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    connect(m_webSocket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors), this, &WssConnection::onSslErrors);
    m_webSocket->open(connection_url);
}

void WssConnection::onConnected()
{
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WssConnection::onTextMessageReceived);
    m_webSocket->sendTextMessage(asset_query);
}

void WssConnection::onTextMessageReceived(QString message)
{
    if(m_filesys->check_update())
    {
        if(file_save_to)
        {
            if(file_save_to -> isOpen())
            {
                file_save_to -> close();
                delete file_save_to;
            }
        }

        file_save_to = new QFile(*(m_filesys->get_current_filepath()), this);
        file_save_to->open(QIODevice::Append | QIODevice::Unbuffered);
    }

    if(message.length() > max_msg_length)
        return;

    emit newMessage(asset_id, message);

    json_parser->handle_json(message);

    if(json_parser->get_time())
    {
        QByteArray array_to_write;
        array_to_write += QString::number((long)json_parser->get_time()) + QString(" ") + QString::number(json_parser->get_value()) + QString("\n");
        file_save_to->write(array_to_write);
        emit newParsedMessage(asset_id, json_parser->get_time(), json_parser->get_value());
    }
}

void WssConnection::onSslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    m_webSocket->ignoreSslErrors();
}
