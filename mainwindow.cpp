#include "mainwindow.h"
#include "controlpanel.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QFile>

const int max_messages = 100;
const int tcp_status_max_height = 100;
const int min_length = 700;
const QString assets_file_path("../wss_qutation_client/assets.txt");

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    checkbox_column = 0;
    checkbox_row = 0;


    tcp_actions_log = new QPlainTextEdit;
    tcp_actions_log->setMaximumBlockCount(max_messages);
    tcp_actions_log->setMaximumHeight(tcp_status_max_height);
    tcp_actions_log->setMinimumWidth(min_length);
    tcp_actions_log->setReadOnly(true);
    tcp = new TcpActions(tcp_actions_log);

    QVBoxLayout* main_vbox = new QVBoxLayout();
    QScrollArea* log_scroll = new QScrollArea;
    QScrollArea* checkbox_scroll = new QScrollArea;
    QWidget* scroll_log_wdgt = new QWidget;
    QWidget* checkbox_wdgt = new QWidget;
    checkbox_layout = new QGridLayout;

    status_fields_vbox = new QVBoxLayout(scroll_log_wdgt);

    checkbox_scroll->setWidgetResizable(true);
    checkbox_scroll->setWidget(checkbox_wdgt);
    checkbox_wdgt->setLayout(checkbox_layout);

    log_scroll->setWidgetResizable(true);
    log_scroll->setWidget(scroll_log_wdgt);

    main_vbox->addWidget(new QLabel("<H1 align = \"center\">WSS Quotes Client</H1>"));
    main_vbox->addWidget(new QLabel("<H3 align = \"center\">TCP actions log</H3>"));
    main_vbox->addWidget(tcp_actions_log);
    main_vbox->addWidget(new QLabel("<H3 align = \"center\">Send Asset Quotes</H3>"));
    main_vbox->addWidget(log_scroll);
    main_vbox->addWidget(new QLabel("<H3 align = \"center\">Asset Quotes Log</H3"));
    main_vbox->addWidget(checkbox_scroll);

    loadStoredConnections();
    setLayout(main_vbox);
}

void MainWindow::loadStoredConnections()
{
    QFile assets_file(assets_file_path);

    if(!assets_file.exists())
    {
        tcp_actions_log->appendPlainText(QString("Check Assets File existance and restart the program!"));
        return;
    }

    if(!assets_file.open(QIODevice::ReadOnly))
    {
        tcp_actions_log->appendPlainText(QString("Can not open Assets File. Check it restart the program!"));
        return;
    }

    while(!assets_file.atEnd())
    {
        QString asset_data = assets_file.readLine();
        asset_data.remove('\n');
        QStringList asset_parameters = asset_data.split(";",  QString::SkipEmptyParts);

        assetQuoteService* new_asset = new assetQuoteService;
        new_asset->asset_id = asset_parameters.value(0).toInt();
        new_asset->asset_name = asset_parameters.value(1);
        new_asset->first_query = asset_parameters.value(2);
        new_asset->asset_status = asset_parameters.value(3).toInt();
        new_asset->asset_controlpanel = new ControlPanel(&new_asset->asset_name);

        assets.insert(new_asset->asset_id, new_asset);

        addNewCheckbox(new_asset);

        if(asset_parameters.value(3).toInt())
        {
            constructNewConnection(new_asset->asset_id, new_asset->asset_name, new_asset->first_query);
        }
    }
}

void MainWindow::addNewCheckbox(assetQuoteService* asset)
{
    QCheckBox* checkbox = new QCheckBox;
    checkbox->setText(asset->asset_name);
    checkbox->setChecked((bool)asset->asset_status);
    asset_checkbox.insert(checkbox, asset->asset_id);

    if(checkbox_column == 3)
    {
        checkbox_column = 0;
        checkbox_row++;
    }

    checkbox_layout->addWidget(checkbox, checkbox_row, checkbox_column++);
    QObject::connect(checkbox, &QCheckBox::clicked, this, &MainWindow::slotCheckboxClicked);
}

void MainWindow::slotNewMessage(int asset_id, QString msg)
{
    assetQuoteService* service = assets.value(asset_id);
    service->asset_controlpanel->addLogMsg(msg);
}

void MainWindow::constructNewConnection(int asset_id, QString& name, QString& query)
{
    WssConnection* connection = new WssConnection;
    assetQuoteService* service = assets.value(asset_id);
    service->asset_connection = connection;

    connection->setConnectionConfiguration(asset_id, name, query);
    status_fields_vbox->addLayout(service->asset_controlpanel->getAssetLayout());
    service->asset_controlpanel->showLabel();

    QThread* new_asset_thread = new QThread;
    service->asset_thread = new_asset_thread;
    asset_thread.insert(new_asset_thread, service->asset_id);
    connection->moveToThread(new_asset_thread);

    QObject::connect(connection, &WssConnection::newParsedMessage, tcp, &TcpActions::slotNewMessage);
    QObject::connect(connection, &WssConnection::newMessage, this, &MainWindow::slotNewMessage);
    QObject::connect(new_asset_thread, &QThread::started, connection, &WssConnection::slotThreadStart);
    new_asset_thread->start();
}

void MainWindow::slotCheckboxClicked()
{
    QCheckBox* checkbox_sender = (QCheckBox*)sender();

    int asset_id = asset_checkbox.value(checkbox_sender);
    assetQuoteService* sender_service = assets.value(asset_id);

    if(checkbox_sender->isChecked())
    {
        sender_service->asset_status = 1;
        constructNewConnection(sender_service->asset_id, sender_service->asset_name, sender_service->first_query);
    }
    else
    {
        shutExistingConnection(sender_service);
        sender_service->asset_status = 0;
    }
}

void MainWindow::shutExistingConnection(assetQuoteService* sender_service)
{
    if(sender_service->asset_status)
    {
        status_fields_vbox->removeItem(sender_service->asset_controlpanel->getAssetLayout());
        sender_service->asset_controlpanel->hideLabel();
        asset_thread.remove(sender_service->asset_thread);
        sender_service->asset_thread->exit();
        sender_service->asset_thread->deleteLater();
        sender_service->asset_thread = nullptr;
        sender_service->asset_connection = nullptr;
    }
}

MainWindow::~MainWindow()
{
    QFile assets_file(assets_file_path);
    assets_file.open(QIODevice::WriteOnly);

    QList<assetQuoteService*> list = assets.values();

    int size = assets.size();

    //do not need to delete checkbuttons because they become children of QWidget, need to close and delete threads;
    for(int i = 0; i < size; i++)
    {
        assets_file.write((QString::number(list[i]->asset_id) + QString(";")
                           + list[i]->asset_name + QString (";")
                           + list[i]->first_query + QString ("; ")
                           + QString::number(list[i]->asset_status) + QString ("\n")).toLatin1());
        shutExistingConnection(list[i]);
        delete list[i];
    }

    assets_file.close();
    QList<assetQuoteService*> values = assets.values();
    size = values.size();

    for(int i = 0; i < size; i++)
    {
        if(!values.value(i)->asset_status)
            delete values.value(i)->asset_controlpanel;
    }

    delete tcp;
}
