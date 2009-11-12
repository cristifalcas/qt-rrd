#ifndef PAGEGENERATEGRAPH_H
#define PAGEGENERATEGRAPH_H

#include "pages.h"

#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QFormLayout>

class PageGenerateGraph : public Pages
{
    Q_OBJECT

public:
    PageGenerateGraph(QWidget * parent = 0);
    ~PageGenerateGraph();
    void draw();
    void updateTime(quint64 min, quint64 max);

private:
    QComboBox *comboboxPeriod;
    QListWidget *listwidgetPics;
    QPushButton *pushbuttonAdd, *pushbuttonGenerate, *pushbuttonClear, *pushButtonSelectStatistics;
    QDateEdit *dateeditPeriodStart, *dateeditPeriodEnd;
    QFormLayout *formlayoutPeriod;

    void connects();
    void layout();
    void init();
};

#endif // PAGEGENERATEGRAPH_H
