#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "extractstatistics.h"
#include "messages.h"

#include <QThread>
#include <QMap>

class mythread : public QThread
{
        Q_OBJECT
public:
    mythread();
    ~mythread();
    void setDataFromStatsConfig(const QMap<QString,QMap<QString,QString> >);
    void run();
    int returncode();

private:
    ExtractStatistics *extract;
    Messages *message;
    int retcode;

public slots:
    void setFiles(QStringList);
    void stopthread();
    void finextract(int);
};

#endif // MYTHREAD_H
