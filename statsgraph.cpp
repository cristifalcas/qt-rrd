#include "statsgraph.h"

#include <QHBoxLayout>
#include <QMessageBox>

const QString statsGraph::stringWorkStatus[] = {
    "nothing",        //xxNOTHING,
    "extract",        //    xxEXTRACT,
    "create",        //    xxCREATE,
    "update",        //    xxUPDATE,
    "done",         //      xxDONE
    "cancel"        //    xxCANCEL,
};

statsGraph::statsGraph()
{
    MY_DEBUG;

    statusBar = new QStatusBar;

    listWidget=new QListWidget;

    listwidgetitemGeneralConfiguration = new QListWidgetItem(listWidget);
    listwidgetitemGeneralConfiguration->setText("Configuration");
    listwidgetitemGeneralConfiguration->setIcon(QIcon(stringDirImages+"config.png"));

    listwidgetitemExtractStats = new QListWidgetItem(listWidget);
    listwidgetitemExtractStats->setText("Extract Stats");
    listwidgetitemExtractStats->setIcon(QIcon(stringDirImages+"update.png"));

    listwidgetitemGenerateGraph = new QListWidgetItem(listWidget);
    listwidgetitemGenerateGraph->setText("Generate Graph");
    listwidgetitemGenerateGraph->setIcon(QIcon(stringDirImages+"generate.png"));

    stackedWidget=new QStackedWidget;

    widgetstackedGeneralConfiguration= new PageGeneralConfiguration(this);
    widgetstackedExtractStats = new PageExtractStats(this);
    widgetstackedGenerateGraph = new PageGenerateGraph(this);

    parser = new ParsingStats;
    extract = new mythread;
    rrdprogram = new RRDTool;
    inforrd = RRDInfo::instance();
    message = Messages::instance();
    frommsgbox = false;

    status = stNOTHING;
}

statsGraph::~statsGraph()
{
    MY_DEBUG;
}

void statsGraph::draw()
{
    MY_DEBUG;

    connects();
    init();
    layout();
}

void statsGraph::init()
{
    MY_DEBUG;

    widgetstackedGeneralConfiguration->draw();
    widgetstackedExtractStats->draw();
    widgetstackedGenerateGraph->draw();

    stackedWidget->addWidget(widgetstackedGeneralConfiguration);
    stackedWidget->addWidget(widgetstackedExtractStats);
    stackedWidget->addWidget(widgetstackedGenerateGraph);

    listWidget->setIconSize(QSize(96,84));
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setMaximumWidth(140);
    listWidget->setMinimumSize(140,350);
    listWidget->setSpacing(10);
    frommsgbox = true;
    listWidget->setCurrentItem(0);
    listWidget->setCurrentRow(0);
}

void statsGraph::connects()
{
    MY_DEBUG;

    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(updatePage(QListWidgetItem *, QListWidgetItem *)),Qt::QueuedConnection);
    connect(widgetstackedExtractStats, SIGNAL(newFiles(QStringList)),
            extract, SLOT(setFiles(QStringList)));
    
    connect(widgetstackedGeneralConfiguration,SIGNAL(enableMain(bool)),
            listWidget,SLOT(setEnabled(bool)));
    connect(widgetstackedGeneralConfiguration,SIGNAL(enableGraph(bool)),
            widgetstackedGenerateGraph,SLOT(setEnabled(bool)));
//start/stop processes/threads
    connect(widgetstackedExtractStats, SIGNAL(startPushedSignal()),
            this, SLOT(begin()));
    connect(widgetstackedExtractStats, SIGNAL(cancelPushedSignal()),
            this, SLOT(cancel()));
//print in message box
    connect(message, SIGNAL(messageBox(QString)),
            this, SLOT(printMessageBox(QString)));
    connect(message, SIGNAL(statusMessage(QString,int)),
            statusBar, SLOT(showMessage(QString,int)));
    connect(message, SIGNAL(updateInfo(QString,int)),
            widgetstackedExtractStats, SLOT(updateLabels(QString, int)));
//threads, processes
    connect(extract, SIGNAL(started()),
            this, SLOT(startWork()));
    connect(rrdprogram, SIGNAL(started()),
            this, SLOT(startWork()));
    connect(extract, SIGNAL(finished()),
            this, SLOT(stopWork()));
    connect(extract, SIGNAL(terminated()),
            this, SLOT(terminated()));
    connect(rrdprogram, SIGNAL(finished()),
            this, SLOT(stopWork()));
}

void statsGraph::begin()
{
    MY_DEBUG;

    if ( parser->getData(widgetstackedGeneralConfiguration->getCurrentStatsConfig()).isEmpty() ){
        printMessageBox("No configuration files in directory " + stringDirStatsFiles);
    }else{
        switch (widgetstackedGeneralConfiguration->getDBStatus()){
            case ftFAIL:{
                    message->impossible();
                    break;
                }
            case ftOVERWRITE:{
                    int ret = QMessageBox::warning(this, "File already exists", \
                                                   "Last chance... File\n" + inforrd->getDatabasePath() + \
                                                   "\nwill be overwriten!", \
                                                   QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
                    switch (ret) {
                    case QMessageBox::Ok:
                        status = statsGraph::stEXTRACT;
                        inforrd->setDatabaseOverwrite(true);

                        extract->start();
                        break;
                    case QMessageBox::Cancel:
                        inforrd->setDatabaseOverwrite(false);
                        frommsgbox = true;
                        stackedWidget->setCurrentIndex(listWidget->row(listwidgetitemGeneralConfiguration));
                        listWidget->setCurrentItem(listwidgetitemGeneralConfiguration);
                        break;
                    default:
                        // should never be reached
                        message->impossible();
                        break;
                    }
                    break;
                }
            case ftNEW:{
                    status = stEXTRACT;
                    extract->start();
                    break;
                }
            case ftUPDATE:{
                    break;
                }
            default:{
                    // should never be reached
                    message->impossible();
                    break;
                }
        }
    }
}

void statsGraph::cancel()
{
    MY_DEBUG;

    status = stCANCEL;

    if ( extract->isRunning() ) {
        extract->stopthread();
        inforrd->clear();
    }
    if ( rrdprogram->isRunning() ){
        rrdprogram->stop();
    }
}

void statsGraph::terminated()
{
    MY_DEBUG;

    status = stCANCEL;
    message->sendLog("Extracting was terminated");

    setEnable(true);
}

void statsGraph::startWork()
{
    MY_DEBUG;

    message->sendLog("Start work from " + stringWorkStatus[status]);

    if ( status == statsGraph::stEXTRACT ){
        setEnable(false);
    }
}

void statsGraph::setEnable(bool val)
{
    MY_DEBUG;

    mutex.lock();
    listWidget->setEnabled(val);
    widgetstackedExtractStats->enableWidgets(val);
    widgetstackedGeneralConfiguration->setEnabled(val);
    mutex.unlock();
}

void statsGraph::stopWork()
{
    MY_DEBUG;

    message->sendLog("Stop work from " + stringWorkStatus[status]);

    switch (status){
        case statsGraph::stNOTHING:
            message->sendLog("How the fuck did we stopped nothing?");
            status = stCANCEL;
        break;
        case statsGraph::stEXTRACT:
            if ( !extract->returncode() ){
                message->sendLog("Start to create RRD file.");
                status = stCREATE;
                rrdprogram->create();
            }else{
                message->sendLog("We had an unexpected error in extract.");
                status = stCANCEL;
                setEnable(true);
            }
        break;
        case statsGraph::stCREATE:
            if ( !rrdprogram->returncode() ){
                message->sendLog("Start to update RRD database.");
                status = stUPDATE;
                rrdprogram->update();
            }else{
                message->sendLog("We had an unexpected error in rrd.");
                status = stCANCEL;
                setEnable(true);
            }
        break;
        case statsGraph::stUPDATE:
            if ( !rrdprogram->returncode() ){
                message->sendLog("We are fucking done.");
                status = stDONE;
                setEnable(true);
            }else{
                message->sendLog("We had an unexpected error in rrd.");
                status = stCANCEL;
                setEnable(true);
            }
        break;
        case statsGraph::stDONE:
            message->sendLog("Done.");
            setEnable(true);
        break;
        case statsGraph::stCANCEL:
            message->sendLog("Cancel all shit.");
            setEnable(true);
        break;
        default:
            message->impossible();
        break;
    }
}

void statsGraph::printMessageBox(QString msg)
{
    MY_DEBUG;

    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

void statsGraph::layout()
{
    MY_DEBUG;

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(listWidget);
    hlayout->addWidget(stackedWidget,1);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(statusBar);
    setLayout(vlayout);
}

void statsGraph::updatePage(QListWidgetItem *crt, QListWidgetItem *prev)
{
    MY_DEBUG;

    widgetstackedGeneralConfiguration->getDBStatus();

    if ( widgetstackedGeneralConfiguration->getDBStatus() == ftOVERWRITE && !frommsgbox && !inforrd->isOverwrite() \
         && crt == listWidget->item(listWidget->row(listwidgetitemExtractStats)) ){
        frommsgbox = false;
        int ret = QMessageBox::warning(this, "File already exists", \
                                       "File\n" + inforrd->getDatabasePath() + "\nwill be overwriten!", \
                                       QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        switch (ret) {
        case QMessageBox::Ok:
            if (!crt){
                crt = prev;
            }
            inforrd->setDatabaseOverwrite(true);
            stackedWidget->setCurrentIndex(listWidget->row(crt));
            listWidget->setCurrentItem(crt);
            break;
        case QMessageBox::Cancel:
            inforrd->setDatabaseOverwrite(false);
            frommsgbox = true;
            stackedWidget->setCurrentIndex(listWidget->row(listwidgetitemGeneralConfiguration));
            listWidget->setCurrentItem(listwidgetitemGeneralConfiguration);
            break;
        default:
            // should never be reached
            message->impossible();
            break;
        }
    }else{
        frommsgbox = false;
        if (!crt){
            crt = prev;
        }
        stackedWidget->setCurrentIndex(listWidget->row(crt));
        listWidget->setCurrentItem(crt);
    }

    parseFiles();
}

void statsGraph::parseFiles()
{
    MY_DEBUG;

    QStringList stringlistStatsConfig = QDir::QDir(stringDirStatsFiles).entryList(QDir::Files,QDir::Name);

    if (!stringlistStatsConfig.isEmpty()){
        parser->empty();

        QStringListIterator it(stringlistStatsConfig);
        while (it.hasNext()){
            parser->parseFile(it.next());
        }
    }else{
        statusBar->showMessage("No files in directory " + stringDirStatsFiles,3000);
        message->sendLog("No files in directory " + stringDirStatsFiles);
        parser->empty();
    }

    widgetstackedGeneralConfiguration->setStatsConfigs(stringlistStatsConfig);
    extract->setDataFromStatsConfig(
            parser->getData(widgetstackedGeneralConfiguration->getCurrentStatsConfig()));
    inforrd->setDataFromStatsConfig(
            parser->getData(widgetstackedGeneralConfiguration->getCurrentStatsConfig()));

}
