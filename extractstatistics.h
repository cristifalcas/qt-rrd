#ifndef EXTRACTSTATISTICS_H
#define EXTRACTSTATISTICS_H

#include "commons.h"
#include "rrdinfo.h"
#include "messages.h"

#include <QStringList>
#include <QDate>
#include <QTimer>

class ExtractStatistics : public QObject
{
    Q_OBJECT

public:
    ExtractStatistics();
    ~ExtractStatistics();
    void setDataFromStatsConfig(const QMap<QString,QMap<QString,QString> >);
    void stopExtract();

private:

    enum typeStatsFields {
        exDATE,
        exTIME,
        exVALUE
    };

    RRDInfo *inforrd;
    Messages * message;
    //filename->section-> key,value
    QMap<QString,QMap<QString,QString> > mapConfigFilesDataValues;
    QFile file;
    QString line;
    QStringList stringlistStatisticsFiles;
    QDate date;
    QTime time;
    QDateTime datetime;
    QTimer *timer;
    bool stop;

    int linenumber, filenumber, exitstatus;

    bool getTime(QString);
    bool getDate(QString, QString);
    void grepValuesFromLine(QString);
    void errorMessageExtract(QString);

public slots:
    void setFiles(QStringList);
    void parseFiles();

private slots:
    void percent();

signals:
    void done(int );
};

#endif // EXTRACTSTATISTICS_H
