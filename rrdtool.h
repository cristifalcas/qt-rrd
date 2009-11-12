#ifndef RRDTOOL_H
#define RRDTOOL_H

#include <QProcess>
#include <QDate>
#include <QMutex>

#include "commons.h"
#include "rrdinfo.h"
#include "messages.h"

class RRDTool : public QObject
{
    Q_OBJECT

public:
    RRDTool();
    ~RRDTool();

    void create();
    void update();
    void fetch(quint64,quint64,quint64);
    void graph();
    void resize();
    void tune();
    void info();
    void last();
    void first();
    void dump();
    void restore();

    void stop();
    bool isRunning();
    void waitforFinished();
    int returncode();
    QString getError();
    QString getOutput();
    QString getExpectedError();
    QString getExpectedOutput();

private:
    QStringList stringArguments;
    QProcess *process;
    QString stringError, stringOutput, stringExpectedError, stringExpectedOutput, here;
    QTime time;
    void createUpdatePairs();
    void run();

    RRDInfo *inforrd;
    Messages * message;
    QMap<quint64, QStringList> maprrd;

private slots:
    void finish();
    void start();

signals:
    void started();
    void finished();
};

#endif // RRDTOOL_H
