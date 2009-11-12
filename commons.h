#ifndef COOMONS_H
#define COOMONS_H

#include <iostream>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QDebug>
using namespace std;
//#define MY_DEBUG qDebug() << __FILE__ << __PRETTY_FUNCTION__
#define MY_DEBUG
#define MY_WARN qWarning()

#define RRA_STEP_MULTIPLIER 3
#define MAX_QINT64 (qint64)9223372036854775807
#define MAX_QUINT64 (quint64)18446744073709551615
#define MIN_QINT64 MAX_QINT64+1
#define MIN_QUINT64 MAX_QUINT64+1

enum typeStatsKeys {
    xEXPRESSION,
    xDATAFIELDS,
    xSEPARATOR,
    xDATEFORMAT,
    xINTERVAL,
    xDATASOURCETYPE,
    xCONSOLIDATIONFUNCTION,
    xALWAYSLAST
} ;

//used only in common.cpp for stringDSType
enum typeDSType {
    yGAUGE,
    yCOUNTER,
    yDERIVE,
    yABSOLUTE,
    yDSTYPELAST
};
//used only in common.cpp for stringValidateCF
enum typeCF{
    zAVERAGE,
    zMIN,
    zMAX,
    zALL,
    zCFLAST
};

enum typeValid{
    ftFAIL = 0,
    ftNEW,
    ftUPDATE,
    ftOVERWRITE
};

//rows is calculated, the rest are from configuration widgets
struct typeRRA {
    QString consolidationfunction;
    double xfilefactor;
    quint64 steps, rows;
};

//min and max are U; the rest we get from program
struct typeDS {
    QString type, name, min, max;
    quint64 heartbeat;
    QMap <quint64, double> values;
};

const extern QString stringStatsKeys[xALWAYSLAST];
const extern QString stringStatsValues[xALWAYSLAST];
const extern QString stringValidateDSType[yDSTYPELAST];
const extern QString stringValidateCF[zCFLAST];

const extern QString stringDirCurrent;
const extern QString stringDirImages;
const extern QString stringDirStatsFiles;

const extern QString stringRegExpSeparator;
const extern QString stringRegExpDate;
const extern QString stringRegExpDataFields;
const extern QString stringRegExpTime;
const extern QString stringRegExpHeartBeat;
const extern QString stringRegExpDataSourceType;

const extern QString stringProgramRRD;

void myMessageOutput(QtMsgType type, const char *msg);

void errorMessage(QString file, QString line, int linenr, QString section, QString msg, QtMsgType type = QtWarningMsg);

#endif // COOMONS_H
