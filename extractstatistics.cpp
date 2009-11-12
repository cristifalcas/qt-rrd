#include "extractstatistics.h"



ExtractStatistics::ExtractStatistics()
{
    MY_DEBUG;

    linenumber = 0;
    filenumber = 0;

    inforrd = RRDInfo::instance();
    message = Messages::instance();
    stop = false;
    timer = new QTimer();
    if (!connect(timer, SIGNAL(timeout()),
                 this, SLOT(percent()), Qt::DirectConnection)) {
        qDebug("VThread::run(): connect (timer) returned false.");
    }
}

void ExtractStatistics::percent()
{
    //MY_DEBUG;
    
    quint64 percent = 0;
    if ( file.isOpen() ){
        QString str;
        if ( file.size() ){
            percent = 100 - (quint64)file.bytesAvailable()*100/file.size();
            str = "File nr " + QString::number(filenumber) +
                  " of " + QString::number(stringlistStatisticsFiles.count());

            message->sendUpdateInfo(str, percent);
        }
    }
}

void ExtractStatistics::parseFiles()
{
    MY_DEBUG;

    timer->start(20);
    stop = false;

    if ( mapConfigFilesDataValues.isEmpty() ){
        message->sendMessageBox("We should not be here. But...\nNo configuration files found in " + stringDirStatsFiles);
        exitstatus = 1;
    }else{
        filenumber = 0;
        inforrd->clear(-1);

        QStringListIterator it(stringlistStatisticsFiles);
        while ( it.hasNext() && !stop ){
            file.setFileName(it.next());

            if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ){
                message->sendLog("Could not open file " + file.fileName());
                message->sendStatusMessage("Could not open file " + file.fileName(), 3000);
                return;
            }else{
                QTime t;t.start();
                linenumber = 0;
                filenumber++;

                while ( !file.atEnd() && !stop ){
                    line = file.readLine();
                    linenumber++;

                    grepValuesFromLine(line);
                }
                file.close();

                QString str;
                str = "Finish extracting from " + file.fileName() + " in " +
                      QString::number(((double)t.elapsed()/1000)) + " seconds.";
                message->sendStatusMessage(str ,3000);
                message->sendLog(str);
            }
        }
    }

    if ( stop ){
        message->sendLog("Received stop.");
        exitstatus = 3;
        inforrd->clear(-1);
    }else{
        if ( inforrd->isEmptyValues() ){
            message->sendStatusMessage("No valid data in given files.",5000);
            message->sendLog("No valid data.");
            exitstatus = 2;
        }else{
            message->sendLog("Finished files.");
            exitstatus = 0;
        }
    }
    timer->stop();
    emit done(exitstatus);
}

#define GET_VALUE_MAP(x) it.value().value(stringStatsKeys[x])
#define GET_FIELD_FILE(x) stringlistLineSplit.at(it.value().value(stringStatsKeys[xDATAFIELDS]).split(" ").at(x).toInt()-1)
void ExtractStatistics::grepValuesFromLine(QString line)
{
    //MY_DEBUG;

    QString str;
    QStringList stringlistLineSplit, datafields;
    //section-> key, value
    QMapIterator<QString, QMap<QString,QString> > it(mapConfigFilesDataValues);

    while ( it.hasNext() ){
        it.next();
        if ( line.contains( QRegExp(GET_VALUE_MAP(xEXPRESSION))) ) {
            //remove all extra spaces
            line = line.simplified();

            //split the line
            stringlistLineSplit = line.split(QRegExp(GET_VALUE_MAP(xSEPARATOR)));
            datafields = GET_VALUE_MAP(xDATAFIELDS).split(" ");

            //check if the line has enough fields
            if ( stringlistLineSplit.count() < datafields[0].toInt() ||
                 stringlistLineSplit.count() < datafields[1].toInt() ||
                 stringlistLineSplit.count() < datafields[2].toInt()){
                errorMessageExtract("number of line fields: " + QString::number(stringlistLineSplit.count())
                                    + " is smaller then max of datafields: " + GET_VALUE_MAP(xDATAFIELDS));
                break;
            }

            //check id date is ok
            str = GET_FIELD_FILE(exDATE);

            if ( getDate(str, GET_VALUE_MAP(xDATEFORMAT)) ){

                //check if time is ok
                str = GET_FIELD_FILE(exTIME);

                if ( getTime(str) ){
                    datetime.setDate(date);
                    datetime.setTime(time);

                    bool ok = false;
                    double value = GET_FIELD_FILE(exVALUE).toDouble(&ok);

                    if (ok){
                        inforrd->setDSValues(it.key(),datetime.toTime_t(),value);
                    }
                    else{
                        errorMessageExtract("value \"" + GET_FIELD_FILE(exVALUE) + "\"");
                        break;
                    }
                }else{
                    errorMessageExtract("time \"" + GET_FIELD_FILE(exTIME) + "\"");
                    break;
                }
            }else{
                errorMessageExtract("date \"" + GET_FIELD_FILE(exDATE) + "\" with format " + GET_VALUE_MAP(xDATEFORMAT));
                break;
            }
        }else{
            //ignored line
        }
    }
}

bool ExtractStatistics::getDate(QString val, QString format)
{
    //MY_DEBUG;

    QStringList listdate = val.replace(QRegExp(stringRegExpDate)," ").simplified().split(" ");
    if ( listdate.size() == 3 ){
        int year_pos = format.toLower().split(" ").indexOf(QRegExp("yyyy|yy"));
        int month_pos = format.toLower().split(" ").indexOf(QRegExp("mm"));
        int day_pos = format.toLower().split(" ").indexOf(QRegExp("dd"));

        if ( year_pos >= 0 && day_pos >= 0 && month_pos >= 0 ){
            date.setDate(listdate.at(year_pos).toInt(),
                         listdate.at(month_pos).toInt(),
                         listdate.at(day_pos).toInt());
        }else{
            return false;
        }

        if (date.isValid()){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool ExtractStatistics::getTime(QString val)
{
    //MY_DEBUG;

    QStringList list = val.replace(QRegExp(stringRegExpTime)," ").simplified().split(" ");
    if ( list.size() == 3 ){
        time.setHMS(list.at(0).toInt(),
                    list.at(1).toInt(),
                    list.at(2).split(",").at(0).toInt());

        if ( time.isValid() ){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

void ExtractStatistics::errorMessageExtract(QString str)
{
    MY_DEBUG;

    message->sendLog("In file " + file.fileName() + " at line:\n\t" + line + \
                      "\tline number " + QString::number(linenumber) + \
                      "\n\t" + str +  " is wrong and the line will be ignored");
}

void ExtractStatistics::setDataFromStatsConfig( const QMap<QString,QMap<QString,QString> > data)
{
    MY_DEBUG;

    mapConfigFilesDataValues = data;
}

void ExtractStatistics::setFiles(QStringList filestoextract)
{
    MY_DEBUG;

    stringlistStatisticsFiles = filestoextract;
}

void ExtractStatistics::stopExtract()
{
    MY_DEBUG;

    stop = true;
}

ExtractStatistics::~ExtractStatistics()
{
    MY_DEBUG;
}
