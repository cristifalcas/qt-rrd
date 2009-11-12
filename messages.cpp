#include "messages.h"
#include "commons.h"

Messages *Messages::m_instance = 0;

Messages::Messages()
{
    MY_DEBUG;
}

void Messages::sendMessageBox(QString string)
{
    MY_DEBUG;

    mutex.lock();
    emit messageBox(string);
    mutex.unlock();
}

void Messages::sendUpdateInfo(QString string, int number)
{
    //MY_DEBUG;

    mutex.lock();
    emit updateInfo(string, number);
    mutex.unlock();
}

void Messages::sendStatusMessage(QString string, int number)
{
    MY_DEBUG;

    mutex.lock();
    emit statusMessage(string, number);
    mutex.unlock();
}

void Messages::sendLog(QString string)
{
    MY_DEBUG;

    mutex.lock();
    MY_WARN << string;
    mutex.unlock();
}

void Messages::impossible()
{
    MY_DEBUG;

    mutex.lock();
    QString msg = "This place should be forbiden. Something is wrong with my flow.";
    sendLog(msg);
    mutex.unlock();
}
