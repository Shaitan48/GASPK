#include "TriggerWatchDog.h"
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>
#include <QTimer> // Added QTimer

TriggerWatchDog::TriggerWatchDog(qlonglong id,QObject *parent,const QString &path, const QString &mask, int interval) : Trigger(id, parent),
    m_path(path), m_mask(mask),m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &TriggerWatchDog::checkDirectory);
    qDebug() << "WatchDog trigger created. path: " + path + ", mask: " + mask + ", interval " + QString::number(interval);
}
TriggerWatchDog::~TriggerWatchDog()
{
    m_timer->stop();
    delete m_timer;
    qDebug() << "WatchDog trigger deleted.";
}
void TriggerWatchDog::start()
{
    m_timer->start(m_interval*1000);
    qDebug() << "WatchDog trigger started.";
}
void TriggerWatchDog::stop()
{
    m_timer->stop();
    qDebug() << "WatchDog trigger stopped.";
}
void TriggerWatchDog::checkDirectory()
{
    QDir dir(m_path);
    if(dir.exists()){
        QStringList files = dir.entryList(QStringList() << m_mask, QDir::Files);
        if (!files.isEmpty()) {
            qDebug() << "File with mask " + m_mask + " founded in directory " + m_path + " " + files.join(", ");
        }else {
            qDebug() << "File with mask " + m_mask + " not founded in directory " + m_path ;
        }

    }else{
        qDebug() << "Directory " + m_path + " not found.";
    }
}

QString TriggerWatchDog::path() const
{
    return m_path;
}

QString TriggerWatchDog::mask() const
{
    return m_mask;
}

int TriggerWatchDog::interval() const
{
    return m_interval;
}
