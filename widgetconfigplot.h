#ifndef WIDGETCONFIGPLOT_H
#define WIDGETCONFIGPLOT_H

#include "commons.h"
#include "pages.h"

#include <QWidget>
#include <QFormLayout>
#include <QSpinBox>

class WidgetConfigPLOT : public Pages
{
    Q_OBJECT

public:
    WidgetConfigPLOT();
    ~WidgetConfigPLOT();
    void draw();

private:
    void connects();
    void layout();

    QFormLayout *formlayout;
    QSpinBox *spinboxStep, *spinboxHeartBeat, *spinboxXFactor;
};

#endif // WIDGETCONFIGPLOT_H
