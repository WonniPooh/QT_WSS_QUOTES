#include "statisticsfilesystem.h"

const QString ASSETS_FOLDER_NAME("ASSETS");

#define FOLDER_ACTIONS(folder_name) if(!stat_filepath.cd(folder_name))\
                                    {\
                                        stat_filepath.mkdir(folder_name);\
                                        if(!stat_filepath.cd(folder_name))\
                                        {\
                                            qDebug("Can't create " + folder_name.toUtf8() + " folder");\
                                            exit(0);\
                                        }\
                                    }

const QString* StatisticsFileSystem::get_current_filepath()
{
    return &current_filepath;
}

int StatisticsFileSystem::check_update()
{
    int updated = 0;

    QDate current_date = QDate::currentDate();

    if(current_date.day() != prev_date.day())
    {
        prev_date = current_date;
        build_new_path();
        updated = 1;
    }

    return updated;
}

void StatisticsFileSystem::build_new_path()
{
    stat_filepath = QDir::homePath();

    FOLDER_ACTIONS(ASSETS_FOLDER_NAME);
    FOLDER_ACTIONS(QString("A" + QString::number(asset_to_service)));
    FOLDER_ACTIONS(QString("YEARS"));
    FOLDER_ACTIONS(QString("Y" + QString::number(prev_date.year())));
    FOLDER_ACTIONS(QString("MONTH"));
    FOLDER_ACTIONS(QString("M" + QString::number(prev_date.month())));
    FOLDER_ACTIONS(QString("DAYS"));
    current_filepath = stat_filepath.absolutePath() +
                       "/A" + QString::number(asset_to_service) +
                       "Y" + QString::number(prev_date.year()) +
                       "M" + QString::number(prev_date.month()) +
                       "D" + QString::number(prev_date.day()) + ".txt";


}

StatisticsFileSystem::StatisticsFileSystem(qint16 serviced_asset)
{
    asset_to_service = serviced_asset;
    prev_date = {};
}
