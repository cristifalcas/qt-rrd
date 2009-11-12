#include "parsingstats.h"

#include <QRegExp>

enum typeParsingContext {
    kNOCONTEXT,
    kGLOBAL,
    kSECTIONHEADER,
    kBODY,
    kEMPTYLINE,
    kCOMMENT,
    kINVALID
} enumCrtContext, enumLineContext;

static const QString stringParsingContext[]={
        "no context",//kNOCONTEXT
        "global context",//kGLOBAL
        "section header",//kSECTIONHEADER
        "body",//kBODY
        "empty line",//kEMPTYLINE
        "comment",//kCOMMENT
        "invalid context"//kINVALID
};

ParsingStats::ParsingStats()
{
    MY_DEBUG;

    stringMagic = "magicstring=stats";
    stringSectionName = QString::null;

    message = Messages::instance();
}

void ParsingStats::parseFile(const QString filename)
{
    MY_DEBUG;

    file.setFileName(stringDirStatsFiles + filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        message->sendLog("Could not open file " + file.fileName());
        return;
    }

    crtLineNr = 0;
    enumCrtContext = kNOCONTEXT;

    while (!file.atEnd()) {
        QString line = file.readLine();
        if (line.endsWith('\n')){
            line.remove(line.length()-1,1).trimmed();
        }
        crtLineNr++;
        enumLineContext = kINVALID;

        getLineContext(line);

        if ( isValidContext() ){
            //don't change the context for what we ignore
            if ( !(enumLineContext == kCOMMENT) && !(enumLineContext == kEMPTYLINE) ){
                enumCrtContext = enumLineContext;
            }

            //we don't like it if we have a section without a body
            if ( !file.atEnd() || (enumCrtContext != kSECTIONHEADER) ){
                //is valid, is not eof, we should get some data
                if ( enumLineContext == kBODY ){
                    mapSectionData.insert(stringSectionName, extractData(line));
                }
            }else{
                error.clear();
                error << "At line" << line << ", line number" << QString::number(crtLineNr) << \
                        "we have" << stringParsingContext[enumCrtContext] << ". Section without body";
                errorMessage(error.join(" "));
                enumCrtContext = kINVALID;
                break;
            }
        }else{
            error.clear();
            error << "At line" << line << ", line number" << QString::number(crtLineNr) << \
                    "we have" << stringParsingContext[enumCrtContext] << ". Invalid context";
            errorMessage(error.join(" "));
            enumCrtContext = kINVALID;
            break;
        }
    }

    if ( enumCrtContext != kINVALID ) {
        mapFilesData.insert(file.fileName(),mapSectionData);
    }
    file.close();

    mapSectionData.clear();

    addDefaults();
}

void ParsingStats::getLineContext(const QString& line)
{
    //MY_DEBUG;

    if ( line == stringMagic ){
        enumLineContext = kGLOBAL;
    }else{
        if ( line.isEmpty() ){
            enumLineContext = kEMPTYLINE;
        }else{
            if ( line.startsWith('#') || line.startsWith(';') || line.startsWith('%') ){
                enumLineContext = kCOMMENT;
            }else{
                if ( line.startsWith('[') && line.endsWith(']') ){
                    enumLineContext = kSECTIONHEADER;
                    int a,b;
                    a=line.indexOf('[');
                    b=line.lastIndexOf(']');
                    stringSectionName=line.mid(a+1,b-1);
                    mapKeyValue.clear();
                }else{
                    QRegExp rx("^([a-z]|[A-Z])");
                    if (line.contains(rx)){
                        enumLineContext = kBODY;
                    }else{
                        enumLineContext = kINVALID;
                        errorMessage(line + " " + QString::number(crtLineNr) + \
                                     ". Invalid context with getLineContext");
                    }
                }
            }
        }
    }
}

bool ParsingStats::isValidContext()
{
    //MY_DEBUG;

    bool valid = false;
    switch (enumCrtContext){
        case kCOMMENT:
        case kEMPTYLINE:
        valid = true;
        break;

        case kNOCONTEXT:
        switch (enumLineContext){
            case kGLOBAL:
            valid = true;
            break;

            default:
            valid = false;
            break;
        }
        break;

        case kGLOBAL:
        switch (enumLineContext){
            case kSECTIONHEADER:
            case kCOMMENT:
            case kEMPTYLINE:
            valid = true;
            break;

            default:
            valid = false;
            break;
        }
        break;

        case kSECTIONHEADER:
        switch (enumLineContext){
            case kBODY:
            case kCOMMENT:
            case kEMPTYLINE:
            valid = true;
            break;

            default:
            valid = false;
            break;
        }
        break;

        case kBODY:
        switch (enumLineContext){
            case kBODY:
            case kSECTIONHEADER:
            case kCOMMENT:
            case kEMPTYLINE:
            valid = true;
            break;

            default:
            valid = false;
            break;
        }
        break;

        default:
        valid = false;
        break;
    }
    return valid;
}

QMap<QString,QString> ParsingStats::extractData(const QString line)
{
    //MY_DEBUG;

    if ( enumCrtContext != kINVALID ){
        QString key, value;
        key = line.section('=',0,0).trimmed().toLower();
        value = line.section('=',1).trimmed();
        QStringList allValidKeys;
        for (unsigned int i=0;i<sizeof(stringStatsKeys)/sizeof(QString);i++){
            allValidKeys << stringStatsKeys[i];
        }

        int i = allValidKeys.indexOf(key);
        if ( i >= 0 && checkLine((typeStatsKeys)i, value) ){
            mapKeyValue.insert(key, value);
        }else{
            if ( i >= 0 ){
                if ( !stringStatsValues[i].isEmpty() ){
                    mapKeyValue.insert(stringStatsKeys[i], stringStatsValues[i]);
                    error << "\n\tUsing default value for key" << stringStatsKeys[i] << \
                                 "with value of" << stringStatsValues[i];
                }
            }else{
                error.clear();
                error << "\n\tkey" << key << "is not valid";
            }
            errorMessage(error.join(" "));
        }
        return mapKeyValue;
    }
    //no compiler warning
    QMap<QString,QString> tmp;
    tmp.clear();
    return tmp;
}

bool ParsingStats::checkLine(typeStatsKeys type, QString &value)
{
    //MY_DEBUG;

    error.clear();
    error << "\n\tat section" << stringSectionName << "\n\tkey" << stringStatsKeys[xDATAFIELDS] << \
            "with value" << value;

    switch (type){
        case xEXPRESSION:
        return true;
        break;
        case xDATAFIELDS:
        return checkDatafields(value);
        break;
        case xSEPARATOR:
        return checkSeparator(value);
        break;
        case xDATEFORMAT:
        return checkDates(value);
        break;
        case xINTERVAL:
        return checkInterval(value);
        break;
        case xDATASOURCETYPE:
        return checkDataSourceType(value);
        break;
        case xCONSOLIDATIONFUNCTION:
        return checkConsolidationFunction(value);
        break;
        default:
        return false;
        break;
    }
}

//for datafields, compact them all and check if they are numbers. That is 3 numbers
bool ParsingStats::checkDatafields(QString &value)
{
    //MY_DEBUG;

    QStringList list = value.replace(QRegExp(stringRegExpDataFields)," ").simplified().split(" ");

    error << "must be composed of 3 numbers.";
    if ( list.count() == 3 ){
        foreach (value, list){
            if ( !value.toInt() ){
                return false;
            }
        }
    }else{
        return false;
    }
    value = list.join(" ");
    return true;
}

//for the separator check for ", ' and remove them
bool ParsingStats::checkSeparator(QString &value)
{
    //MY_DEBUG;

    if ( QRegExp(stringRegExpSeparator).exactMatch(value) ){
        value = value.remove(0,1).remove(value.length()-2,1);
        return true;
    }else{
        error << "should have been enclosed by \", \'.";
        return false;
    }
}

//for dates, replace the separator with space
//we must have 3 fields
bool ParsingStats::checkDates(QString &value)
{
    //MY_DEBUG;

    value = value.replace(QRegExp(stringRegExpDate)," ").simplified();
    if ( value.count(" ") == 2 ) {
        QString tmp = value;
        tmp.replace(QRegExp("yy|yyyy"),"");//year
        tmp.replace(QRegExp("mm"),"");//month
        tmp.replace(QRegExp("dd"),"");//day
        if ( tmp.simplified().isEmpty() ){
            return true;
        }else{
            error << "does not have correct format.";
            return false;
        }
    }else {
        error << "does not have enough fields.";
        return false;
    }
}

bool ParsingStats::checkInterval(QString &value)
{
    //MY_DEBUG;

    if ( QRegExp(stringRegExpHeartBeat).exactMatch(value) ){
        return true;
    }else{
        return false;
    }
}

bool ParsingStats::checkDataSourceType(QString &value)
{
    //MY_DEBUG;

    QStringList list;
    for ( unsigned int i=0; i<sizeof(stringValidateDSType)/sizeof(QString); i++){
        list << stringValidateDSType[i];
    }

    QRegExp regexp;
    regexp.setPattern(list.join("|"));
    if ( regexp.exactMatch(value.toUpper()) ){
        return true;
    }else{
        error << "should have been something from:" << list.join(", ");
        return false;
    }
}

bool ParsingStats::checkConsolidationFunction(QString &value)
{
    //MY_DEBUG;

    QStringList list;
    for ( unsigned int i=0; i<sizeof(stringValidateCF)/sizeof(QString); i++){
        list << stringValidateCF[i];
    }

    QRegExp regexp;
    regexp.setPattern(list.join("|"));
    if ( regexp.exactMatch(value.toUpper()) ){
        return true;
    }else{
        error << "should have been something from:" << list.join(", ");
        return false;
    }
}
void ParsingStats::addDefaults()
{
    //MY_DEBUG;

    QMapIterator<QString,QMap<QString,QString> > itSection(mapFilesData.value(file.fileName()));
    while ( itSection.hasNext() ){
        enumCrtContext=kNOCONTEXT;
        itSection.next();

        QString str;

        if ( checkEmptyValues(itSection.value(), itSection.key()) ){
            for ( int i=0;i<xALWAYSLAST;i++){
                if ( !mapKeyValue.contains(stringStatsKeys[i]) )
                mapKeyValue.insert(stringStatsKeys[i], stringStatsValues[i]);
            }
        }

        if ( enumCrtContext != kINVALID ){
            mapSectionData.insert(itSection.key(),mapKeyValue);
        }
        mapKeyValue.clear();
    }

    if ( mapSectionData.isEmpty() ){
        errorMessage("there is no usable section and the file will be ignored.");
        mapFilesData.remove(file.fileName());
    }else{
        mapFilesData.insert(file.fileName(), mapSectionData);
    }
    mapSectionData.clear();
}

//Insert defaults for all empty values
bool ParsingStats::checkEmptyValues(QMap<QString, QString> keyValue, QString section)
{
    MY_DEBUG;

    for (int i=0;i<xALWAYSLAST;i++){
        //check if any value is empty and complain where we have to
        if ( keyValue.value(stringStatsKeys[i]).isEmpty() ){
            //if the default one is empty, this is bad
            if (stringStatsValues[i].isEmpty()){
                errorMessage("key " + stringStatsKeys[i] + " is missing and the section will be ignored.");
                //enumCrtContext = kINVALID;
                mapSectionData.remove(section);
                return false;
            }else{
                //if we don't have the key, insert the default one
                mapKeyValue.insert(stringStatsKeys[i],stringStatsValues[i]);
            }
        }else{
            //not empty, we keep what we have
            mapKeyValue.insert(stringStatsKeys[i],keyValue.value(stringStatsKeys[i]));
        }
    }
    return true;
}

QMap<QString,QMap<QString,QString> > ParsingStats::getData(QString statsconfig)
{
    MY_DEBUG;

    return mapFilesData.value(stringDirStatsFiles + statsconfig);
}

void ParsingStats::printInfo()
{
    MY_DEBUG;

    QMapIterator<QString, QMap<QString,QMap<QString,QString> > > itFiles(mapFilesData);
    while (itFiles.hasNext()) {
        itFiles.next();
        MY_WARN << "- files" << itFiles.key();

        QMapIterator<QString,QMap<QString,QString> > itSection(itFiles.value());
        while ( itSection.hasNext() ){
            itSection.next();
            MY_WARN << "-- section" << itSection.key() << "--- key:value" << itSection.value();
        }
    }
}

bool ParsingStats::isEmpty()
{
    MY_DEBUG;

    return mapFilesData.isEmpty();
}

void ParsingStats::empty()
{
    MY_DEBUG;

    mapFilesData.clear();
}

void ParsingStats::errorMessage(QString msg)
{
    MY_DEBUG;

    QStringList all;
    all << "In file" << file.fileName() << msg;
    message->sendLog(all.join(" ").toLatin1().data());
}

ParsingStats::~ParsingStats()
{
    MY_DEBUG;
}
