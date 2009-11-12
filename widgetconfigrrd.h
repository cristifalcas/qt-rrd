#ifndef WIDGETCONFIGRRD_H
#define WIDGETCONFIGRRD_H

#include "commons.h"
#include "rrdinfo.h"
#include "pages.h"

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSignalMapper>

class WidgetConfigRRD : public Pages
{
    Q_OBJECT

public:
    WidgetConfigRRD();
    ~WidgetConfigRRD();
    void draw();

private:
    void connects();
    void layout();
    void init();

enum enumToolTip {
    cxSTEP,
    cxXFACTOR,
    cxCONSOLIDFNCT,
};
    QSpinBox *spinboxStep;
    QDoubleSpinBox *doublespinboxXFactor;
    QComboBox *comboboxConsolidationFunction;
    QSignalMapper *signalMapper;

    RRDInfo *inforrd;

private slots:
    void updateRRDConfig(int);
};

#endif // WIDGETCONFIGRRD_H
