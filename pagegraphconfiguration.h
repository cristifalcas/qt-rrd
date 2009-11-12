#ifndef PAGEGRAPHCONFIGURATION_H
#define PAGEGRAPHCONFIGURATION_H

#include "pages.h"

#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QGridLayout>

class PageGraphConfiguration : public Pages
{
    Q_OBJECT

public:
    PageGraphConfiguration(QWidget * parent = 0);

    void draw();

public slots:
    void changeWindow();

private:
    QLabel *labelStep, *labelHeartBeat, *labelPeriod;
    QSpinBox *spinStep, *spinHeartBeat, *spinPeriod;
    QPushButton *pushgetfile;
    QRadioButton *radiobuttonRrd, *radiobuttonPlot;

    QGroupBox *groupboxConfig;
    QGridLayout *gridlayoutConfig;

    void connects();
    void layout();
};

#endif // PAGEGRAPHCONFIGURATION_H
