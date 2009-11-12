#include "rrdinfo.h"

#define RELATIVE_HEARTBEAT 3

RRDInfo *RRDInfo::m_instance = 0;

//normal, 10 minutes. 1 hour, 12 hours, 1 day, weekly
const unsigned int nr_rra = 6;

RRDInfo::RRDInfo()
{
    MY_DEBUG;

    rra = new typeRRA[nr_rra];
    datasource = NULL;
    datasourcesize = 0;
    overwritedb = false;

    stringDatabasePath = stringDirCurrent + "/database";

    period["minute"] = spMinute;
    period["minutes"] = spMinute;
    period["hour"] = spHour;
    period["hours"] = spHour;
    period["day"] = spDay;
    period["days"] = spDay;
    period["week"] = spWeek;
    period["weeks"] = spWeek;
    period["month"] = spMonth;
    period["months"] = spMonth;
    period["year"] = spYear;
    period["years"] = spYear;
}

void  RRDInfo::setRRAXFileFactor(double val)
{
    MY_DEBUG;

    rra[0].xfilefactor = val;
}

void RRDInfo::setRRASteps(quint64 val)
{
    MY_DEBUG;

    rra[0].steps = val;
}

void RRDInfo::setDatabasePath(QString val)
{
    MY_DEBUG;

    if ( val != stringDatabasePath ){
        stringDatabasePath = val;
        overwritedb = false;
    }
}

void RRDInfo::setRRAConsolidationFunction(QString val)
{
    MY_DEBUG;

    for ( unsigned int i = 0; i < nr_rra; i++){
        rra[i].consolidationfunction = val.toUpper();
    }
}


void RRDInfo::printInfo(QString dsname)
{
    MY_DEBUG;

    for (unsigned int i=0;i<datasourcesize;i++){
        if ( datasource[i].name == dsname || dsname.isEmpty() ){
            MY_WARN << "DataSource name" << datasource[i].name;
            MY_WARN << "\tHeartBeat" << datasource[i].heartbeat;
            //MY_WARN << "\tMax" << datasource[i].max;
            //MY_WARN << "\tMin" << datasource[i].min;
            MY_WARN << "\tType" << datasource[i].type;

//            QMap<quint64, double>::const_iterator it = datasource[i].values.constBegin();
//            while (it != datasource[i].values.constEnd()) {
//                MY_WARN << "\t\t" << it.key() << ": " << it.value();
//                ++it;
//            }
        }
    }
}


void RRDInfo::setDSValues(QString dsname, quint64 time, double value)
{
    //MY_DEBUG;

    mutex.lock();
    for (unsigned int i=0;i<datasourcesize;i++){
        if ( datasource[i].name == dsname ){
            datasource[i].values.insert(time,value);
        }
    }

    mutex.unlock();
}

/*
  in map we have datasource name (somekey, somevalue)
*/
void RRDInfo::setDataFromStatsConfig(const QMap<QString,QMap<QString,QString> > data)
{
    MY_DEBUG;

//keep rra, because it will only be updated from config widgets and only re-do datasource
    if ( !datasource ){
        delete datasource;
    }

    int i = 0;
    datasource = new typeDS[data.count()];
    QMapIterator<QString, QMap<QString, QString> > it(data);
    while ( it.hasNext() ){
        it.next();
        datasource[i].name = it.key();
        datasource[i].type = it.value().value(stringStatsKeys[xDATASOURCETYPE]).toUpper();
        datasource[i].heartbeat = it.value().value(stringStatsKeys[xINTERVAL]).toInt() * RELATIVE_HEARTBEAT;
        datasource[i].min = 'U';
        datasource[i].max = 'U';

        i++;
    }
    datasourcesize = i;
    createRRA();
}

void RRDInfo::clear(int nr)
{
    MY_DEBUG;

    mutex.lock();
    if ( nr < 0 ){
        for (unsigned int i=0;i<datasourcesize;i++){
            datasource[i].values.clear();
        }
    }else if ( nr < (int)datasourcesize ){
        datasource[nr].values.clear();
    }else{
        MY_WARN << "Can't delete datasource" << QString::number(nr) <<
                "because we have only" << QString::number(datasourcesize-1) << "datasources.";
    }
    mutex.unlock();
}

bool RRDInfo::isEmptyValues()
{
    MY_DEBUG;

    bool ret = true;
    for (unsigned int i=0;i<datasourcesize;i++){
        if ( !datasource[i].values.isEmpty() ){
            ret = false;
            break;
        }
    }
    return ret;
}

quint64 RRDInfo::getDBStep()
{
    MY_DEBUG;

    quint64 max = 1;

    for (unsigned int i=0;i<datasourcesize;i++){
        if ( datasource[i].heartbeat > max ){
            max = datasource[i].heartbeat;
        }
    }
    return max / RELATIVE_HEARTBEAT;
}

const QString RRDInfo::getDatabasePath()
{
    MY_DEBUG;

    return stringDatabasePath;
}

quint64 RRDInfo::getMinTime()
{
    MY_DEBUG;

    quint64 min = MAX_QUINT64;

    for (unsigned int i=0;i<datasourcesize;i++){
        if ( datasource[i].values.begin().key() < min ){
            min = datasource[i].values.begin().key();
        }
    }

    return min;
}

quint64 RRDInfo::getDatasourcesNumber()
{
    MY_DEBUG;

    return datasourcesize;
}

const typeDS RRDInfo::getDatasource(unsigned int i)
{
    //MY_DEBUG;

    if ( i > datasourcesize ){
        typeDS invalid;
        invalid.heartbeat=0;
        invalid.name="";
        invalid.type="invalid";
        invalid.values[-1]=-1;
        return invalid;
    }else{
        return datasource[i];
    }
}

/*
  we have 3 times that matter:
    - step from database: Specifies the base interval in seconds with which data will be fed into the RRD.
    - heartbeat from datasource : defines the maximum number of seconds that may pass between two updates
            of this data source before the value of the data source is assumed to be *UNKNOWN*
   - steps from rra (NUMBER): defines how many of these primary data points are used to build a consolidated data point
                which then goes into the archive.
*/
void RRDInfo::createRRA()
{
    MY_DEBUG;

    int dbstep =  getDBStep();
    int x = 0, y = 0;
//every x time for y period
//formula for step after first rra: transform the x time in seconds and divide by db step
//formula for rows: transform the y period in seconds and divide by x time in seconds
    //those should be already set
//    rra[0].consolidationfunction = ;
//    rra[0].steps = ;
//    rra[0].xfilefactor = ;
    //we build 1 point from STEPS values, each every STEP
    //STEP is max of interval
    //STEPS will come from the rrd widget
//ex: interval is 5, step will be 5, steps is 2: we keep 1 point every 10s, we will have 267840 rows
    //every interval for 3 months
    x = rra[0].steps * dbstep;
    y = timeinseconds(3, "months");
    rra[0].rows = (int)(y/x);

    //after this, we presume each pdp is exact and correct
    //we set fxx minimum
    //we set steps for the period we need
    //cf is the same for all
//ex: we have STEP=5, so we need to average 600/STEP points = 120, we will have (10min*6)hours * 24*31*6 rows
    //every 5 min for 6 months
    x = timeinseconds(5, "minutes");
    y = timeinseconds(6,"months");
    rra[1].steps = (int)(x/dbstep);
    rra[1].rows = (int)(y/x);
    rra[1].xfilefactor = 0.2;

    //15 minutes for 1 year
    x = timeinseconds(15, "minutes");
    y = timeinseconds(1,"year");
    rra[2].steps = (int)(x/dbstep);
    rra[2].rows = (int)(y/x);
    rra[2].xfilefactor = 0.1;

    //30 minutes for 2 years
    x = timeinseconds(30,"minutes");
    y = timeinseconds(2,"years");
    rra[3].steps = (int)(x/dbstep);
    rra[3].rows = (int)(y/x);
    rra[3].xfilefactor = 0.01;

    //1 hour for 5 years
    x = timeinseconds(1, "hour");
    y = timeinseconds(5, "years");
    rra[4].steps = (int)(x/dbstep);
    rra[4].rows = (int)(y/x);
    rra[4].xfilefactor = 0.01;

    //4 hours for 10 years
    x = timeinseconds(4, "hours");
    y = timeinseconds(10, "years");
    rra[5].steps = (int)(x/dbstep);
    rra[5].rows = (int)(y/x);
    rra[5].xfilefactor = 0.01;
}

int RRDInfo::timeinseconds(int nr, QString time)
{
    int minute = 60;
    int hour = 60 * minute;
    int day = 24 * hour;
    int week = 7 * day;
    int month = 31 * day;
    int year = 12 * month;

    switch (period.value(time)){
        case spMinute:
            return nr * minute;
        break;
        case spHour:
            return nr * hour;
        break;
        case spDay:
            return nr * day;
        break;
        case spWeek:
            return nr * week;
        break;
        case spMonth:
            return nr * month;
        break;
        case spYear:
            return nr * year;
        break;
        default:
            return 0;
        break;
    }
}

const typeRRA RRDInfo::getRRA(unsigned int i)
{
    MY_DEBUG;

    if ( i > nr_rra ){
        typeRRA invalid;
        invalid.consolidationfunction="invalid";
        invalid.xfilefactor=100;
        invalid.rows=-1;
        invalid.steps=-1;
        return invalid;
    }else{
        return rra[i];
    }
}

quint64 RRDInfo::getRRAsNumber()
{
    MY_DEBUG;

    return nr_rra;
}

void RRDInfo::setDatabaseOverwrite(bool val)
{
    MY_DEBUG;

    overwritedb = val;
}

bool RRDInfo::isOverwrite()
{
    MY_DEBUG;

    return overwritedb;
}
