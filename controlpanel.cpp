#include "controlpanel.h"
#include "QTextEdit"
#include "QPushButton"
#include "QHBoxLayout"
#include "QLabel"
#include <QString>

const int max_messages = 100;

ControlPanel::ControlPanel(QString *asset_name, QWidget *parent) : QWidget(parent)
{
    QPushButton* show_log_btn = new QPushButton("Show Log");
    m_control_panel = new QVBoxLayout;
    income_log = new QPlainTextEdit;
    panel_title = new QLabel;

    income_log->setMaximumBlockCount(max_messages);
    show_button_state = false;

    if(!asset_name)
        panel_title->setText("Default Name");
    else
        panel_title->setText(QString("<H3>") + *asset_name + QString("</H3>"));


    connect(show_log_btn, &QPushButton::clicked, this, &ControlPanel::showButtonPressed);

    m_control_panel->addWidget(panel_title, 0, Qt::AlignCenter);
    m_control_panel->addWidget(show_log_btn);
    m_control_panel->addWidget(income_log);

    income_log->hide();
}

ControlPanel::~ControlPanel()
{
    delete m_control_panel;
}

void ControlPanel::showButtonPressed()
{
    QPushButton* sender_button = qobject_cast<QPushButton*>(sender());

    if(show_button_state)
    {
        sender_button->setText("Show Log");
        income_log->hide();
    }
    else
    {
        sender_button->setText("Hide Log");
        income_log->show();
        m_control_panel->setSizeConstraint(QLayout::SetMinimumSize);
    }

    show_button_state = !show_button_state;
}

QVBoxLayout* ControlPanel::getAssetLayout()
{
    return m_control_panel;
}

void ControlPanel::showLabel()
{
    panel_title->setVisible(true);
}

void ControlPanel::hideLabel()
{
    panel_title->hide();
}

void ControlPanel::addLogMsg(QString &message)
{
    income_log->appendPlainText(message);
}
