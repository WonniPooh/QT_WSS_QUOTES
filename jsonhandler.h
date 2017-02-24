#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QObject>

class jsonHandler : public QObject
{
    Q_OBJECT
public:
    explicit jsonHandler(QObject *parent = 0);
    void handle_json(QString &input);
    double get_value();
    double get_time();

private:
    double close_value;
    int time;

};

#endif // JSONHANDLER_H
