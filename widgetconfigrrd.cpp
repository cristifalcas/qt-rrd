#include "widgetconfigrrd.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

QString strTooltip[] =
{
    "Defines how many of the primary data points are used to build a\n"
    "consolidated data point. For simplicity, we will use in this program\n"
    "as at least quadruple of the max interval defined (from the sections\n"
    "in the configuration files. So be carefull how you combine them).",

    "The xfiles factor defines what part of a consolidation \n"
    "interval may be made up from *UNKNOWN* data while the \n"
    "consolidated value is still regarded as known. It is given \n"
    "as the ratio of allowed *UNKNOWN* PDPs to the number of\n"
    "PDPs in the interval. Thus, it ranges from 0 to 1 (exclusive).",

    "Consolidation function. You want average"
};

WidgetConfigRRD::WidgetConfigRRD()
{
    MY_DEBUG;

    spinboxStep = new QSpinBox;
    doublespinboxXFactor = new QDoubleSpinBox;

    comboboxConsolidationFunction = new QComboBox;

    signalMapper = new QSignalMapper;

    inforrd = RRDInfo::instance();
}

void WidgetConfigRRD::draw()
{
    MY_DEBUG;

    init();
    connects();
    layout();
}

void WidgetConfigRRD::init()
{
    MY_DEBUG;

    spinboxStep->setMinimum(1);
    spinboxStep->setMaximum(2000);
    spinboxStep->setSuffix(" sec");
    spinboxStep->setAlignment(Qt::AlignRight);
    spinboxStep->setValue(1);
    spinboxStep->setToolTip(strTooltip[cxSTEP]);
    inforrd->setRRASteps(spinboxStep->value());

    doublespinboxXFactor->setRange(0.01,0.99);
    doublespinboxXFactor->setDecimals(5);
    doublespinboxXFactor->setSingleStep(0.01);
    doublespinboxXFactor->setValue(.5);
    doublespinboxXFactor->setToolTip(strTooltip[cxXFACTOR]);
    inforrd->setRRAXFileFactor(doublespinboxXFactor->value());

    for (unsigned int i=0; i<sizeof(stringValidateCF)/sizeof(QString); i++){
        comboboxConsolidationFunction->addItem(stringValidateCF[i]);
    }
    comboboxConsolidationFunction->setToolTip(strTooltip[cxCONSOLIDFNCT]);
    inforrd->setRRAConsolidationFunction(comboboxConsolidationFunction->currentText());
}

void WidgetConfigRRD::connects()
{
    MY_DEBUG;

    connect(signalMapper,
            SIGNAL(mapped(int)),
            this,
            SLOT(updateRRDConfig(int))
            );

    signalMapper->setMapping(spinboxStep, cxSTEP);
    signalMapper->setMapping(doublespinboxXFactor, cxXFACTOR);
    signalMapper->setMapping(comboboxConsolidationFunction, cxCONSOLIDFNCT);

    connect(spinboxStep, SIGNAL(valueChanged(int)),
            signalMapper, SLOT(map()));
    connect(doublespinboxXFactor, SIGNAL(valueChanged(double)),
            signalMapper, SLOT(map()));
    connect(comboboxConsolidationFunction, SIGNAL(currentIndexChanged(int)),
            signalMapper, SLOT(map()));
}

void WidgetConfigRRD::layout()
{
    MY_DEBUG;

    QFormLayout *formlayout = new QFormLayout;

    formlayout->addRow("Step",spinboxStep);
    formlayout->addRow("X Factor",doublespinboxXFactor);
    formlayout->addRow("Consolidation Function",comboboxConsolidationFunction);
    formlayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addLayout(formlayout);
    setLayout(layout);
}

void WidgetConfigRRD::updateRRDConfig(int value)
{
    MY_DEBUG;

    switch (value){
        case cxSTEP:
            inforrd->setRRASteps(spinboxStep->value());
            break;
        case cxXFACTOR:
            inforrd->setRRAXFileFactor(doublespinboxXFactor->value());
            break;
        case cxCONSOLIDFNCT:
            inforrd->setRRAConsolidationFunction(comboboxConsolidationFunction->currentText());
            break;
        default:
            break;
    }
}

WidgetConfigRRD::~WidgetConfigRRD()
{
    MY_DEBUG;
}
