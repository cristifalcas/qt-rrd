#include "pageextractstats.h"
#include <mcheck.h>

#include <QFileDialog>

PageExtractStats::PageExtractStats(QWidget * parent)
{
    MY_DEBUG;

    listwidgetFiles = new QListWidget(parent);
    listwidgetFiles->setSortingEnabled(true);

    pushbuttonFiles = new QPushButton("Files...");
    pushbuttonClearFiles = new QPushButton("Clear List");
    pushbuttonCancel = NULL;
    pushbuttonStart = NULL;

    labelLoadingApp = NULL;
    labelLoadingPercent = NULL;
    labelLoadingFileName = NULL;

    vboxlayoutFiles = new QVBoxLayout;
    gridlayout = new QGridLayout;
mtrace();
    movieLoadingApp = new QMovie("images/loading.gif");
    movieLoadingApp->setSpeed(150);
    message = Messages::instance();
}

void PageExtractStats::initRemovedButtons(const bool createCancel)
{
    MY_DEBUG;

    if ( !createCancel ){
        //delete generate button
        //create cancel button
        //add movie-label
        gridlayout->removeWidget(pushbuttonStart);
        if ( pushbuttonStart ){
            delete pushbuttonStart;
            pushbuttonStart=NULL;
        }

        if ( !pushbuttonCancel ){
            pushbuttonCancel = new QPushButton("Cancel");
        }
        gridlayout->addWidget(pushbuttonCancel,1,0,2,1,Qt::AlignJustify);

        if ( !labelLoadingApp ){
            labelLoadingPercent = new QLabel("""");
            labelLoadingApp = new QLabel("");
            labelLoadingFileName = new QLabel("");
            labelLoadingFileName->setWordWrap(true);
            labelLoadingApp->setMovie(movieLoadingApp);
            movieLoadingApp->jumpToFrame(0);
            movieLoadingApp->start();
        }
        vboxlayoutFiles->insertWidget(vboxlayoutFiles->indexOf(pushbuttonClearFiles),labelLoadingPercent);
        vboxlayoutFiles->insertWidget(vboxlayoutFiles->indexOf(labelLoadingPercent),labelLoadingApp);
        vboxlayoutFiles->insertWidget(vboxlayoutFiles->indexOf(labelLoadingApp),labelLoadingFileName);

        connect(pushbuttonCancel,SIGNAL(clicked()),
                this,SLOT(cancelPushedSlot()));
    }else {
        //delete cancel button
        //create generate button
        //remove movie-label
        gridlayout->removeWidget(pushbuttonCancel);
        if ( pushbuttonCancel ){
            delete pushbuttonCancel;
            pushbuttonCancel = NULL;
        }

        if ( !pushbuttonStart ){
            pushbuttonStart = new QPushButton("Start");
        }
        gridlayout->addWidget(pushbuttonStart,1,0,2,1,Qt::AlignJustify);

        vboxlayoutFiles->removeWidget(labelLoadingPercent);
        vboxlayoutFiles->removeWidget(labelLoadingApp);
        vboxlayoutFiles->removeWidget(labelLoadingFileName);
        if ( labelLoadingApp ){
            delete labelLoadingPercent;
            delete labelLoadingApp;
            delete labelLoadingFileName;
            labelLoadingFileName = NULL;
            labelLoadingApp=NULL;
            labelLoadingPercent = NULL;
            movieLoadingApp->stop();
        }

        connect(pushbuttonStart,SIGNAL(clicked()),
                this,SLOT(startPushedSlot()));
    }
}

void PageExtractStats::connects()
{
    MY_DEBUG;

    connect(pushbuttonFiles, SIGNAL(clicked()),
            this, SLOT(addFiles()));
    connect(pushbuttonClearFiles, SIGNAL(clicked()),
            listwidgetFiles, SLOT(clear()));
}

void PageExtractStats::layout()
{
    MY_DEBUG;

    vboxlayoutFiles->addWidget(pushbuttonFiles);
    vboxlayoutFiles->addStretch(1);
    vboxlayoutFiles->addWidget(pushbuttonClearFiles);

    gridlayout->addWidget(listwidgetFiles,0,0);
    gridlayout->addLayout(vboxlayoutFiles,0,1);
    gridlayout->addWidget(pushbuttonStart,1,0,2,1,Qt::AlignJustify);

    setLayout(gridlayout);
}

void PageExtractStats::draw()
{
    MY_DEBUG;

    initRemovedButtons(true);
    layout();
    connects();
}

void PageExtractStats::addFiles()
{
    MY_DEBUG;

    QStringList strlst = QFileDialog::getOpenFileNames(this,tr("Select files"), "", tr("Any Files (*)"));

    if (!strlst.isEmpty()){
        QStringList::Iterator it = strlst.begin();
        while(it != strlst.end()) {
            if (listwidgetFiles->findItems(*it,Qt::MatchExactly).isEmpty()){
                listwidgetFiles->addItem(*it);
            } else {
                MY_WARN << "File already exists:" << *it;
            }
            ++it;
        }
    }

    QStringList fileslist;
    for (int i=0; i<listwidgetFiles->count(); i++){
        fileslist << listwidgetFiles->item(i)->text();
    }

    emit newFiles(fileslist);
}

void PageExtractStats::enableWidgets(bool value)
{
    MY_DEBUG;

    mutex.lock();
    pushbuttonClearFiles->setEnabled(value);
    pushbuttonFiles->setEnabled(value);
    listwidgetFiles->setEnabled(value);
    initRemovedButtons(value);
    mutex.unlock();
}


void PageExtractStats::startPushedSlot()
{
    MY_DEBUG;

    if ( listwidgetFiles->count()  ) {
        emit startPushedSignal();
    }else{
        message->sendMessageBox("Moar filees??");
        return;
    }
}

void PageExtractStats::cancelPushedSlot()
{
    MY_DEBUG;

    emit cancelPushedSignal();
}

void PageExtractStats::updateLabels(QString string, int number)
{
    //MY_DEBUG;

    mutex.lock();
    if (labelLoadingFileName && labelLoadingPercent){
        labelLoadingFileName->setText(string);
        labelLoadingPercent->setText(QString::number(number));
    }
    mutex.unlock();
}

PageExtractStats::~PageExtractStats()
{
    MY_DEBUG;
}
