#ifndef STATISTICSFILESYSTEM_H
#define STATISTICSFILESYSTEM_H

#pragma once
#include <QDir>
#include <QDate>

class StatisticsFileSystem
{
    public:
        explicit StatisticsFileSystem(qint16 serviced_asset);
        int check_update();
        const QString *get_current_filepath();

    private:
        void build_new_path();
        qint16 asset_to_service;
        QString current_filepath;
        QDate prev_date;
        QDir  stat_filepath;
};

#endif
