#include "pagegenerategraph.h"

#include <QGridLayout>

PageGenerateGraph::PageGenerateGraph(QWidget * parent)
{
    MY_DEBUG;

    comboboxPeriod = new QComboBox(parent);
    listwidgetPics = new QListWidget(parent);

    dateeditPeriodStart = new QDateEdit(parent);
    dateeditPeriodEnd = new QDateEdit(parent);

    pushbuttonAdd = new QPushButton(parent);
    pushbuttonGenerate = new QPushButton(parent);
    pushbuttonClear = new QPushButton(parent);
    pushButtonSelectStatistics = new QPushButton(parent);

    formlayoutPeriod = new QFormLayout;
    formlayoutPeriod->addRow("Start", dateeditPeriodStart);
    formlayoutPeriod->addRow("End", dateeditPeriodEnd);
}

void PageGenerateGraph::connects()
{
    MY_DEBUG;
}

void PageGenerateGraph::init()
{
    MY_DEBUG;

    comboboxPeriod->addItem("Daily");
    comboboxPeriod->addItem("Weekly");
    comboboxPeriod->addItem("Monthly");
    comboboxPeriod->addItem("Full");
    comboboxPeriod->addItem("Custom");

    dateeditPeriodStart->setDisplayFormat("dd MMM yyyy");
    dateeditPeriodStart->setCalendarPopup(true);
    dateeditPeriodEnd->setDisplayFormat("dd MMM yyyy");

    pushbuttonAdd->setText("Add");
    pushbuttonGenerate->setText("Generate");
    pushbuttonClear->setText("Clear");
    pushButtonSelectStatistics->setText("Select statistics...");
}

void PageGenerateGraph::layout()
{
    MY_DEBUG;

    QGridLayout *gridlayout = new QGridLayout;
    gridlayout->addWidget(comboboxPeriod,0,0);
    gridlayout->addLayout(formlayoutPeriod,0,1);

    gridlayout->addWidget(pushButtonSelectStatistics,1,0);
    gridlayout->addWidget(pushbuttonAdd,2,0,1,2);
    gridlayout->addWidget(listwidgetPics,3,0,2,2);
    gridlayout->addWidget(pushbuttonGenerate,5,0);
    gridlayout->addWidget(pushbuttonClear, 4, 2);
    setLayout(gridlayout);
}

void PageGenerateGraph::draw()
{
    MY_DEBUG;

    init();
    layout();
    connects();
}

PageGenerateGraph::~PageGenerateGraph()
{
    MY_DEBUG;
}

void PageGenerateGraph::updateTime(quint64 min, quint64 max)
{
}
