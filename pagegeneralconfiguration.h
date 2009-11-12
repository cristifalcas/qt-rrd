#ifndef PAGEGENERALCONFIGURATION_H
#define PAGEGENERALCONFIGURATION_H

#include "pages.h"
#include "widgetconfigrrd.h"
#include "widgetconfigplot.h"
#include "rrdtool.h"
#include "messages.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDir>
#include <QGroupBox>
#include <QGridLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFileSystemModel>

#include <QStackedWidget>
#include <QComboBox>

class PageGeneralConfiguration : public Pages
{
    Q_OBJECT

public:
    PageGeneralConfiguration(QWidget * parent = 0);
    ~PageGeneralConfiguration();
    void draw();
    void setStatsConfigs(QStringList);
    QString getCurrentStatsConfig();
    int getDBStatus();
    void enableWidgets(bool);

private slots:
    void setSaveDir();
    int isValid();

private:

    QHBoxLayout *layoutComboBox;

    QCompleter *completerDir, *completerFile;
    QFileSystemModel *modelDir, * modelFile;

    QComboBox *comboboxGraphType, *comboboxUpdateDB, *comboboxFiles;
    QPushButton *pushbuttonRRDdir;
    QGroupBox *groupboxFiles, *groupboxConfig;
    QStackedWidget *stackedwidgetConfig;
    QLabel *labelDBUpdate;
    QLineEdit *lineeditRRDdir,*lineeditRRDdatabase;
    QString stringLastUpdateDB;
    QMutex mutex;

    WidgetConfigRRD *configRRD;
    WidgetConfigPLOT *configPLOT;

    RRDInfo *inforrd;
    Messages * message;
    int onlyoneerror;
    typeValid dbstatus;

    void connects();
    void layout();
    void init();
    void completers();
    void updateOverwrite();

    bool isValidRRDInputFile();
    bool isValidRRDProg();
    bool isValidInputFile();

signals:
    void error(QString);
    void enableMain(bool);
    void enableGraph(bool);
};

#endif // PAGEGENERALCONFIGURATION_H
