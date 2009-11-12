#include "pagegeneralconfiguration.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QCompleter>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#define RED 255,80,80
#define GREEN 120,255,100
#define YELLOW 255,255,180
#define WHITE 255,255,255

#if defined (Q_OS_WIN32)
#include <windows.h>
#endif
//windows pid: (qint64)proc->pid()->dwProcessId)

QString stringoverwrite = "Overwrite DB", stringupdate = "Update DB";

PageGeneralConfiguration::PageGeneralConfiguration(QWidget * parent)
{
    MY_DEBUG;

    onlyoneerror = 0;

    modelDir = new QFileSystemModel;
    completerDir = new QCompleter(this);
    completerFile = new QCompleter(this);

    lineeditRRDdir = new QLineEdit(parent);
    lineeditRRDdatabase = new QLineEdit(parent);

    groupboxFiles = new QGroupBox(parent);
    groupboxConfig = new QGroupBox(parent);

    comboboxGraphType = new QComboBox(parent);
    comboboxUpdateDB = NULL;
    comboboxFiles = new QComboBox(parent);

    pushbuttonRRDdir = new QPushButton("...");

    configRRD = new WidgetConfigRRD;
    configPLOT = new WidgetConfigPLOT;
    inforrd = RRDInfo::instance();

    stackedwidgetConfig = new QStackedWidget(parent);

    stringLastUpdateDB = stringoverwrite;

    layoutComboBox = new QHBoxLayout;

    layoutComboBox->addWidget(comboboxGraphType);
    layoutComboBox->addStretch(1);

    message = Messages::instance();
}

void PageGeneralConfiguration::completers()
{
    MY_DEBUG;

    modelDir->setNameFilters(QStringList::QStringList("*"));
    modelDir->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    modelDir->setRootPath("/home/cristi");
    //w->setSorting(QDir::IgnoreCase | QDir::Name);

    completerDir->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completerDir->setModel(modelDir);

//    stringlistCompleterFiles = QDir::QDir(lineeditRRDdir->text(),QString::QString(""),
//                                   QDir::IgnoreCase | QDir::Name,
//                                   QDir::Files).entryList();
//    completerFile = new QCompleter(stringlistCompleterFiles, this);
//    completerFile->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

    QFileSystemModel *q = new QFileSystemModel;

    q->setRootPath("/home/");
    q->setNameFilterDisables(false);
    q->setNameFilters(QStringList::QStringList(""));
    q->setFilter(QDir::Files);
    //q->setSorting(QDir::IgnoreCase | QDir::Name);

    completerFile->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completerFile->setModel(q);
    //lineeditRRDdir->setCompleter(completerDir);
    //lineeditRRDdatabase->setCompleter(completerFile);
}

void PageGeneralConfiguration::init()
{
    MY_DEBUG;

    QString dbpath = inforrd->getDatabasePath();
    lineeditRRDdir->setReadOnly(false);
    lineeditRRDdir->setText(dbpath.left(dbpath.lastIndexOf("/")));

    lineeditRRDdatabase->setReadOnly(false);
    lineeditRRDdatabase->setText(dbpath.mid(dbpath.lastIndexOf("/")+1));

    groupboxFiles->setTitle("Files");
    groupboxConfig->setTitle("Archives Configuration");

    comboboxGraphType->setMaximumWidth(100);
    comboboxGraphType->addItem("RRD");
    comboboxGraphType->addItem("PLOT");

    pushbuttonRRDdir->setFixedSize(24,24);

    configRRD->draw();
    configPLOT->draw();
    stackedwidgetConfig->addWidget(configRRD);
    stackedwidgetConfig->addWidget(configPLOT);

    isValid();
}

void PageGeneralConfiguration::connects()
{
    MY_DEBUG;

    connect(pushbuttonRRDdir, SIGNAL(clicked()),
            this, SLOT(setSaveDir()));
    connect(comboboxGraphType, SIGNAL(activated(int)),
            stackedwidgetConfig, SLOT(setCurrentIndex(int)));

    //some connects to check for the validity
    connect(comboboxGraphType, SIGNAL(highlighted(QString)),
            this, SLOT(isValid()));
    connect(comboboxFiles, SIGNAL(highlighted(QString)),
            this, SLOT(isValid()));
    connect(lineeditRRDdatabase, SIGNAL(editingFinished()),
            this, SLOT(isValid()));
    connect(lineeditRRDdir, SIGNAL(editingFinished()),
            this, SLOT(isValid()));
    connect(lineeditRRDdatabase, SIGNAL(textChanged(QString)),
            this, SLOT(isValid()));
    connect(lineeditRRDdir, SIGNAL(textChanged(QString)),
            this, SLOT(isValid()));
}

void PageGeneralConfiguration::layout()
{
    MY_DEBUG;

    QHBoxLayout *layoutFiles = new QHBoxLayout;
    layoutFiles->addWidget(lineeditRRDdir);
    layoutFiles->addWidget(pushbuttonRRDdir);

    QFormLayout *formlayout, *formlayoutFiles;
    formlayout = new QFormLayout;
    formlayoutFiles = new QFormLayout;
    formlayoutFiles->addRow("Select save dir name",layoutFiles);
    formlayoutFiles->addRow("Select database name",lineeditRRDdatabase);
    groupboxFiles->setLayout(formlayoutFiles);

    QVBoxLayout *layoutConfig = new QVBoxLayout;
    layoutConfig->addWidget(stackedwidgetConfig);
    groupboxConfig->setLayout(layoutConfig);

//layout final
    formlayout->addRow(layoutComboBox);
    formlayout->addRow(groupboxFiles);
    formlayout->addRow(comboboxFiles);
    formlayout->addRow(groupboxConfig);
    formlayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    setLayout(formlayout);
}

void PageGeneralConfiguration::draw()
{
    MY_DEBUG;

    completers();
    init();
    connects();
    layout();
}

/*
  Here we draw or remove the label and combobox for the already existing file
  we receive as string the current text, but we don't use it, because we can reach here from both edits

if the string is not an existing file, we remove, if we have to, the db update widgets

if the string is an existing file, we add the db update widgets, if we have to,
        and connect the update db combobox to colorizeLieEdit
*/
void PageGeneralConfiguration::updateOverwrite()
{
    inforrd->setDatabasePath( lineeditRRDdir->text() + "/" + lineeditRRDdatabase->text() );
    if ( isValidInputFile() ){
        if (!comboboxUpdateDB){
            comboboxUpdateDB = new QComboBox;
            labelDBUpdate = new QLabel("File exists");

            connect(comboboxUpdateDB, SIGNAL(activated(QString)),
                    this, SLOT(isValid()));

            comboboxUpdateDB->addItem(stringoverwrite);
            comboboxUpdateDB->addItem(stringupdate);
            comboboxUpdateDB->setCurrentIndex(comboboxUpdateDB->findText(stringLastUpdateDB));

            layoutComboBox->insertWidget(layoutComboBox->count(),labelDBUpdate);
            layoutComboBox->insertWidget(layoutComboBox->count(),comboboxUpdateDB);
        }
    }else{
        if (comboboxUpdateDB){
            stringLastUpdateDB = comboboxUpdateDB->currentText();
            layoutComboBox->removeWidget(labelDBUpdate);
            layoutComboBox->removeWidget(comboboxUpdateDB);
            delete labelDBUpdate;
            delete comboboxUpdateDB;
            labelDBUpdate = NULL;
            comboboxUpdateDB = NULL;
        }
    }
}

/*
  Check if the existing file is a valid rrd db
  We check for the file just for amusment
*/
bool PageGeneralConfiguration::isValidRRDInputFile()
{
    MY_DEBUG;

    if ( isValidInputFile() ){
        RRDTool *rrdprogram = new RRDTool();

        rrdprogram->info();
        rrdprogram->waitforFinished();

        if ( rrdprogram->returncode() ){
            message->sendLog("File " + inforrd->getDatabasePath() + " is not a RRD database.");
            return false;
        }else{
            message->sendLog("File " + inforrd->getDatabasePath() + " is a valid rrd file.");
            return true;
        }
    }else{
        message->impossible();
        return false;
    }
}

/*
  if we don't have the rrdtool, we can't do anything
*/
bool PageGeneralConfiguration::isValidRRDProg()
{
    MY_DEBUG;

    QFileInfo file( stringProgramRRD );
    if ( file.exists() && file.isFile() ){
        onlyoneerror = 0;
        message->sendStatusMessage("",0);
        return true;
    }else{
        if (!onlyoneerror++){
            QString err = "We can't find rrdtool in \n" +
                               stringProgramRRD + "\n" +
                               "Can't do much without it.";
            message->sendStatusMessage(err, 0);
            message->sendMessageBox(err);
        }
        message->sendLog("RRD program does not exist at " + stringProgramRRD);
        return false;
    }
}

/*
  Does the input file exists?
*/
bool PageGeneralConfiguration::isValidInputFile()
{
    MY_DEBUG;

    QFileInfo file(inforrd->getDatabasePath());
    if ( file.exists() && file.isFile() ){
        return true;
    }else{
        return false;
    }
}

void PageGeneralConfiguration::setStatsConfigs(QStringList configfiles)
{
    QString last = comboboxFiles->currentText();
    comboboxFiles->clear();
    comboboxFiles->insertItems(0, configfiles);
    if ( !last.isEmpty() ){
        comboboxFiles->setCurrentIndex(comboboxFiles->findText(last));
    }
}

void PageGeneralConfiguration::setSaveDir()
{
    MY_DEBUG;

    QString str = QFileDialog::getOpenFileName(this,
                                                    "Select directory",
                                                    inforrd->getDatabasePath());
    if ( str != 0 ){
        QFileInfo file(str);
        lineeditRRDdir->setText(file.absoluteDir().absolutePath());
        lineeditRRDdatabase->setText(file.fileName());
    }
}

/*
  used only if no files in stats dir
*/
void PageGeneralConfiguration::enableWidgets(bool value)
{
    MY_DEBUG;

    mutex.lock();
    if ( dbstatus != ftUPDATE ){
        emit enableGraph(false);
    }else{
        emit enableGraph(value);
    }
    stackedwidgetConfig->setEnabled(value);
    emit enableMain(value);
    mutex.unlock();
}

/*
  We have 4 switches:
  -     input file(inforrd->getDatabasePath()),
        rrd file (validRRDPrg),
        update/overwrite (combobox),
        input file is valid rrd (validRRDFile)
  - rrd file does not exist : fail
  - rrd file exists:
    - input file does not exist: ok, new
    - input file exists
        - combo overwrite : ok, new
        - combo update :
            - is valid rrd: ok, update
            - is not valid rrd: fail
  all other cases should be impossible
  exits: 0=fail, 1=new, 2=update
*/
int PageGeneralConfiguration::isValid()
{
    MY_DEBUG;

    updateOverwrite();

    #define SET_COLOR(x)    palette.setColor( QPalette::Base, QColor::QColor(x) );\
        lineeditRRDdatabase->setPalette(palette);

    QPalette palette( lineeditRRDdatabase->palette() );

    bool validComboBox;// 0,false = update; 1,true = overwrite or new file
    if ( comboboxUpdateDB ){
        if ( comboboxUpdateDB->currentText() == stringupdate ){
            validComboBox = false;
        }else{
            validComboBox = true;
        }
    }else{
        validComboBox = true;
    }

    if ( !isValidRRDProg() ) {
        enableWidgets(false);
        dbstatus = ftFAIL;
    }else{
        if ( !isValidInputFile() ){
            SET_COLOR(WHITE);
            enableWidgets(true);
            dbstatus = ftNEW;
        }else{
            if ( validComboBox ){
                SET_COLOR(YELLOW);
                enableWidgets(true);
                dbstatus = ftOVERWRITE;
            }else{
                if ( isValidRRDInputFile() ){
                    message->sendStatusMessage("",0);
                    SET_COLOR(GREEN);
                    enableWidgets(true);
                    dbstatus = ftUPDATE;
                }else{
                    message->sendStatusMessage("We can't update a non RRD file.\n"
                               "Update the configuration (choose overwrite or select a new file)",0);
                    SET_COLOR(RED);
                    enableWidgets(false);
                    dbstatus = ftFAIL;
                }
            }
        }
    }

    return dbstatus;
}

QString PageGeneralConfiguration::getCurrentStatsConfig()
{
    MY_DEBUG;

    return comboboxFiles->currentText();
}

int PageGeneralConfiguration::getDBStatus()
{
    MY_DEBUG;

    return isValid();
}

PageGeneralConfiguration::~PageGeneralConfiguration()
{
    MY_DEBUG;
}
