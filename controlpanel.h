#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QVBoxLayout>

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QString *asset_name, QWidget *parent = 0);
    ~ControlPanel();
    QPlainTextEdit* getTextEdit();
    void addLogMsg(QString &message);
    QVBoxLayout* getAssetLayout();
signals:


public slots:
    void  showButtonPressed();

private:
    bool show_button_state;
    QVBoxLayout* m_control_panel;
    QPlainTextEdit* income_log;
};

#endif // CONTROLPANEL_H
