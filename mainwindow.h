#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QObject>
#include <QMap>
#include <QCheckBox>
#include <QThread>
#include <QPlainTextEdit>
#include "wssconnection.h"

class QGridLayout;
class QVBoxLayout;
class ControlPanel;

struct assetQuoteService
{
    assetQuoteService() noexcept :
                        asset_id(0), asset_status(0),
                        asset_thread(nullptr),
                        asset_checkbox(nullptr),
                        asset_connection(nullptr)
    {}

    int asset_id;
    QString asset_name;
    QString first_query;
    int asset_status;
    QThread* asset_thread;
    QCheckBox* asset_checkbox;
    WssConnection* asset_connection;
};

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotCheckboxClicked();

private:
    int checkbox_row = 0;
    int checkbox_column = 0;

    void constructNewThread(int asset_id, QString& name, QString& query);
    void shutExistingThread(assetQuoteService *sender_service);

    void addNewCheckbox(assetQuoteService* asset);
    void loadStoredConnections();

    QGridLayout* checkbox_layout;
    QMap<QThread*, int> asset_thread;
    QMap<QCheckBox*, int> asset_checkbox;
    QMap<int, assetQuoteService*> assets;
    QPlainTextEdit* tcp_actions_log;
    QVBoxLayout* status_fields_vbox;
    WssConnection connection;
};

#endif // MAINWINDOW_H
