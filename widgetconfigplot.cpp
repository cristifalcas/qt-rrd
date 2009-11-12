#include "widgetconfigplot.h"

WidgetConfigPLOT::WidgetConfigPLOT()
{
    MY_DEBUG;

    formlayout = new QFormLayout;

    spinboxHeartBeat = new QSpinBox;
    spinboxStep = new QSpinBox;

    formlayout->addRow("nimic facut",spinboxHeartBeat);
    formlayout->addRow("nica nica",spinboxStep);
    formlayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    setLayout(formlayout);
}

void WidgetConfigPLOT::draw()
{
    MY_DEBUG;

    layout();
    connects();
}

void WidgetConfigPLOT::connects()
{
    MY_DEBUG;
}

void WidgetConfigPLOT::layout()
{
    MY_DEBUG;
}

WidgetConfigPLOT::~WidgetConfigPLOT()
{
    MY_DEBUG;
}
