#ifndef RRDINFO_H
#define RRDINFO_H

#include "commons.h"

#include <QStringList>
#include <QMutex>

/*
  Format:
  filename:
    DS1:
    DS2:
    RRA1
    RRA2
    ...
Each "DS" declares a dataset, or a group of values.
Each "RRA" declares an archive of the datasets we'd like to keep for later analyzation.

DS has : ds-name:GAUGE | COUNTER | DERIVE | ABSOLUTE:heartbeat:min:max
RRA has : AVERAGE | MIN | MAX | LAST:xff:steps:rows
*/

class RRDInfo
{
public:
    static RRDInfo *instance()
    {
        if( m_instance == 0 ) m_instance = new RRDInfo();
        return m_instance;
    }

    void setRRAXFileFactor(const double);
    void setRRASteps(const quint64);
    void setRRAConsolidationFunction(const QString);
    void setDSValues(const QString dsname, const quint64 time, const double value);
    void setDataFromStatsConfig(const QMap<QString,QMap<QString,QString> >);
    void setDatabasePath(const QString);
    void setDatabaseOverwrite(bool);

    quint64 getDBStep();
    const QString getDatabasePath();
    quint64 getMinTime();
    quint64 getDatasourcesNumber();
    quint64 getRRAsNumber();
    const typeDS getDatasource(unsigned int);
    const typeRRA getRRA(unsigned int);
    bool isOverwrite();

    void clear(int i=-1);
    bool isEmptyValues();

    void printInfo(QString dsname = "");

private:
    RRDInfo();
    static RRDInfo *m_instance;
    void createRRA();

    typeRRA *rra;
    typeDS *datasource;

    QString stringDatabasePath;
    QMutex mutex;
    quint64 datasourcesize;
    bool overwritedb;
    int timeinseconds(int nr, QString time);

    enum StringPeriod { spMinute,
                       spHour,
                       spDay,
                       spWeek,
                       spMonth,
                       spYear };
    QMap <QString, StringPeriod> period;
};

#endif // RRDINFO_H
