#include "pagegraphconfiguration.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStringList>
#include <QString>

PageGraphConfiguration::PageGraphConfiguration(QWidget * parent)
{
    qDebug()<<__PRETTY_FUNCTION__;

    labelStep = new QLabel("Step");
    labelHeartBeat = new QLabel("Heart Beat");
    labelPeriod = new QLabel("Period");

    pushgetfile = new QPushButton("Generate Graph");

    radiobuttonPlot = new QRadioButton("Plot");
    radiobuttonRrd = new QRadioButton("RRD");
    radiobuttonRrd->setChecked(true);

    groupboxConfig = new QGroupBox();
    gridlayoutConfig = new QGridLayout();

    spinStep = new QSpinBox();
    spinHeartBeat = new QSpinBox();
    spinPeriod = new QSpinBox();

    spinStep->setMinimum(0);
    spinHeartBeat->setMinimum(0);
    spinPeriod->setMinimum(0);

    spinStep->setSuffix(" sec");
    spinStep->setAlignment(Qt::AlignRight);
    spinStep->setValue(10);
    spinStep->setToolTip("pasii intre valorile care se introduc in BD");

    spinHeartBeat->setSuffix(" sec");
    spinHeartBeat->setAlignment(Qt::AlignRight);
    spinHeartBeat->setValue(10);
    spinHeartBeat->setToolTip("heartbeat timpul intre 2 update-uri (macar de 2 ori valoarea dintre puncte)");

    spinPeriod->setMaximum(10000000);
    spinPeriod->setSuffix(" rows");
    spinPeriod->setValue(1000000);
    spinPeriod->setToolTip("10.000 puncte pe zi -> rows pe 1 luna ~ 300.000 (1.000.000 = 3 luni ~ 8MB)");
}

void PageGraphConfiguration::draw()
{
    qDebug()<<__PRETTY_FUNCTION__;

    layout();
    connects();
}

void PageGraphConfiguration::connects()
{
    qDebug()<<__PRETTY_FUNCTION__;

    connect(pushgetfile,SIGNAL(clicked()),this,SLOT(getValuesFromFile()));
}

void PageGraphConfiguration::layout()
{
    qDebug()<<__PRETTY_FUNCTION__;

    int row=-1;

    gridlayoutConfig->addWidget(labelStep,++row,0);
    gridlayoutConfig->addWidget(spinStep,row,1);

    gridlayoutConfig->addWidget(labelHeartBeat,++row,0);
    gridlayoutConfig->addWidget(spinHeartBeat,row,1);

    gridlayoutConfig->addWidget(labelPeriod,++row,0);
    gridlayoutConfig->addWidget(spinPeriod,row,1);

    groupboxConfig->setLayout(gridlayoutConfig);
    groupboxConfig->setTitle("RRD");

    QHBoxLayout *hlayoutradionbuttons = new QHBoxLayout;
    hlayoutradionbuttons->addWidget(radiobuttonRrd);
    hlayoutradionbuttons->addWidget(radiobuttonPlot);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayoutradionbuttons);
    vlayout->addWidget(groupboxConfig);
    vlayout->addStretch(1);
    vlayout->addWidget(pushgetfile);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addLayout(vlayout);
    hlayout->addStretch(1);

    setLayout(hlayout);
}

void PageGraphConfiguration::changeWindow()
{
    qDebug()<<__PRETTY_FUNCTION__;

    if (radiobuttonRrd->isChecked())
    {}
}
