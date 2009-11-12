#ifndef STATSGRAPH_H_INCLUDED
#define STATSGRAPH_H_INCLUDED

#include "pagegeneralconfiguration.h"
#include "pagegenerategraph.h"
#include "pageextractstats.h"
#include "parsingstats.h"
#include "rrdtool.h"
#include "rrdinfo.h"
#include "mythread.h"
#include "messages.h"

#include <QDialog>
#include <QStackedWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QMap>
#include <QStatusBar>

class statsGraph : public QDialog
{
    Q_OBJECT

public:
    statsGraph();
    ~statsGraph();
    void draw();

private:
    enum typeWorkStatus {
        stNOTHING,
        stEXTRACT,
        stCREATE,
        stUPDATE,
        stDONE,
        stCANCEL,
        last
    } status;
    static const QString stringWorkStatus[last];

    QStatusBar *statusBar;
    QStackedWidget *stackedWidget;
    QListWidget *listWidget;
    QListWidgetItem *listwidgetitemGeneralConfiguration,
            *listwidgetitemExtractStats, *listwidgetitemGenerateGraph;
    QString step;
    QMutex mutex;
    bool frommsgbox; //used to not answer second time the message box

    PageGeneralConfiguration *widgetstackedGeneralConfiguration;
    PageExtractStats *widgetstackedExtractStats;
    PageGenerateGraph * widgetstackedGenerateGraph;

    ParsingStats *parser;
    mythread *extract;
    RRDTool *rrdprogram;
    RRDInfo *inforrd;
    Messages *message;

    void init();
    void connects();
    void layout();
    void parseFiles();
    //void disable();
    void setEnable(bool);

public slots:
    void updatePage(QListWidgetItem *crt, QListWidgetItem *prev);
    void printMessageBox(QString);

    void terminated();

    void startWork();
    void stopWork();
    void begin();
    void cancel();
};

#endif // STATSGRAPH_H_INCLUDED
