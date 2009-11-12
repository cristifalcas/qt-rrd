#include "mythread.h"

mythread::mythread()
{
    MY_DEBUG;

    extract = new ExtractStatistics;
    message = Messages::instance();
    connect(extract, SIGNAL(done(int)),
            this, SLOT(finextract(int)));
}

void mythread::setDataFromStatsConfig(const QMap<QString,QMap<QString,QString> > data)
{
    MY_DEBUG;

    extract->setDataFromStatsConfig(data);
}

void mythread::run()
{
    MY_DEBUG;

    extract->parseFiles();
    retcode = exec();
    message->sendLog("Received exit code " + QString::number(retcode));
}

void mythread::setFiles(QStringList list)
{
    MY_DEBUG;

    extract->setFiles(list);
}

void mythread::stopthread()
{
    MY_DEBUG;

    extract->stopExtract();
}

void mythread::finextract(int i)
{
    MY_DEBUG;

    this->exit(i);
}

mythread::~mythread()
{
    MY_DEBUG;
}

int mythread::returncode()
{
    MY_DEBUG;

    return retcode;
}
