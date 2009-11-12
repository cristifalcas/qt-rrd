#ifndef MESSAGES_H
#define MESSAGES_H

#include <QObject>
#include <QMutex>

class Messages : public QObject
{
    Q_OBJECT

public:
    static Messages *instance()
    {
        if( m_instance == 0 ) m_instance = new Messages();
        return m_instance;
    }

    void sendMessageBox(QString);
    void sendUpdateInfo(QString string, int number);
    void sendStatusMessage(QString string, int number);
    void sendLog(QString string);
    void impossible();

signals:
    void messageBox(QString);
    void updateInfo(QString string, int number);
    void statusMessage(QString string, int number);

private:
    Messages();
    static Messages *m_instance;
    QMutex mutex;
};

#endif // MESSAGES_H
