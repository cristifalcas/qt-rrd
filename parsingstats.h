#ifndef PARSINGSTATS_H
#define PARSINGSTATS_H

#include "commons.h"
#include "messages.h"

#include <QFile>
#include <QMap>
#include <QStringList>

class ParsingStats
{
public:
    ParsingStats();
    ~ParsingStats();
    void parseFile(const QString filename);
    bool isEmpty();
    void empty();
    QMap<QString,QMap<QString,QString> > getData(QString);
    void printInfo();

private:
    QFile file;
    QString stringMagic, stringSectionName;
    QStringList error;
    QMap<QString,QString> mapKeyValue;
    QMap<QString,QMap<QString,QString> > mapSectionData;
    QMap<QString,QMap<QString,QMap<QString,QString> > > mapFilesData;
    int crtLineNr;
    Messages *message;

    void getLineContext(const QString&);
    bool isValidContext();
    void addDefaults();
    bool checkLine(typeStatsKeys, QString &value);
    bool checkDatafields(QString &value);
    bool checkSeparator(QString &value);
    bool checkDates(QString &value);
    bool checkInterval(QString &value);
    bool checkDataSourceType(QString &value);
    bool checkConsolidationFunction(QString &value);
    bool checkEmptyValues(QMap<QString, QString> keyValue, QString sectionKey);
    QMap<QString,QString> extractData(const QString line);
    void errorMessage(QString msg);
};

#endif // PARSINGSTATS_H
