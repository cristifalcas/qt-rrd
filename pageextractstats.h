#ifndef PAGEEXTRACTSTATS_H
#define PAGEEXTRACTSTATS_H

#include "pages.h"
#include "extractstatistics.h"
#include "messages.h"

#include <QListWidgetItem>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMovie>
#include <QProcess>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>

class PageExtractStats : public Pages
{
    Q_OBJECT

public:
    PageExtractStats(QWidget *parent = 0);
    ~PageExtractStats();
    void draw();

public slots:
    void addFiles();
    void enableWidgets(bool);
    void startPushedSlot();
    void cancelPushedSlot();
    void updateLabels(QString filename, int percent);

signals:
    void startPushedSignal();
    void cancelPushedSignal();
    void newFiles(QStringList);

private:
    QListWidget *listwidgetFiles;
    QLabel *labelLoadingApp, *labelLoadingPercent, *labelLoadingFileName;
    QPushButton *pushbuttonFiles, *pushbuttonStart,
        *pushbuttonClearFiles, *pushbuttonCancel;
    QMovie *movieLoadingApp;
    QMutex mutex;

    QVBoxLayout *vboxlayoutFiles;
    QGridLayout *gridlayout;
    Messages * message;

    void connects();
    void layout();
    void initRemovedButtons(const bool buttonload);
};

#endif // PAGEEXTRACTSTATS_H
