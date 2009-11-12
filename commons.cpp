#include "commons.h"

const QString stringValidateDSType[] = {
    "GAUGE",
    "COUNTER",
    "DERIVE",
    "ABSOLUTE"
};

const QString stringValidateCF[] = {
    "AVERAGE",
    "MIN",
    "MAX",
    "ALL"
};

//xEXPRESSION
//xDATAFIELDS
//xSEPARATOR
//xDATEFORMAT
//xINTERVAL
//xDATASOURCETYPE

const QString stringStatsKeys[] = {
    "expression",       //xEXPRESSION
    "datafields",       //xDATAFIELDS
    "separator",        //xSEPARATOR
    "dateformat",       //xDATEFORMAT
    "interval",         //xINTERVAL
    "datasourcetype",    //xDATASOURCETYPE
    "consolidationfunction" //xCONSOLIDATIONFUNCTION
};

//if mandatory, leave blank
const QString stringStatsValues[] = {
    "",             //xEXPRESSION
    "1 2 3",        //xDATAFIELDS
    " ",            //xSEPARATOR
    "dd mm yyyy",   //xDATEFORMAT
    "10",           //xINTERVAL
    stringValidateDSType[0],        //xDATASOURCETYPE
    stringValidateCF[0]       //xCONSOLIDATIONFUNCTION
};

const QString stringDirCurrent = QDir::currentPath();
const QString stringDirImages = stringDirCurrent + "/images/";
const QString stringDirStatsFiles = stringDirCurrent + "/stats/";


#if defined (Q_OS_WIN32)
const QString stringProgramRRD = stringDirCurrent + "/rrd/rrdtool.exe";
#else
const QString stringProgramRRD = "/usr/bin/rrdtool";
#endif

const QString stringRegExpSeparator = "(^\".*\"$)|(^'.*'$)";
const QString stringRegExpDate = "/|-|\\.|:";
const QString stringRegExpDataFields = ",|;";
const QString stringRegExpTime = ":";
const QString stringRegExpHeartBeat = "1000|[1-9]\\d{0,2}";
const QString stringRegExpDataSourceType = "";

bool first = true;
void myMessageOutput(QtMsgType type, const char* msg)
{
    QFile file("aaa_out");
    if ( QFileInfo(file.fileName()).exists() && first ){
        first = false;
        file.remove();
    }
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);
    switch (type) {
    case QtDebugMsg:
        if ( !QString::QString(msg).startsWith("kfile") ){
            fprintf(stderr, "Debug: %s\n", msg);
            out << "Debug: " << msg << endl;
        }
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        out << "Warning: " << msg << endl;
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        out << "Critical: " << msg << endl;
        exit(2);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        out << "Fatal: " << msg << endl;
        exit(3);
    }
    file.close();
 }

