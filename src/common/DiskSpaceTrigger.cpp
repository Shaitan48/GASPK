#include "DiskSpaceTrigger.h"
#include <QDebug>
#include <QStorageInfo>
#include <QDir>
#include <QTimer> // Added QTimer

DiskSpaceTrigger::DiskSpaceTrigger(qlonglong id,QObject *parent, const QStringList &disks, int threshold, int interval) : Trigger(id,parent),
    m_disks(disks), m_threshold(threshold), m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &DiskSpaceTrigger::checkDiskSpace);
    QString disksString;
    for(const auto &disk : m_disks){
        disksString += disk + ",";
    }
    qDebug() << "Disk space trigger created. disks: " + disksString + " , threshold: " + QString::number(threshold) + " interval " + QString::number(interval);
}
DiskSpaceTrigger::~DiskSpaceTrigger()
{
    m_timer->stop();
    delete m_timer;
    qDebug() << "Disk space trigger deleted.";
}
void DiskSpaceTrigger::start()
{
    m_timer->start(m_interval*1000);
    qDebug() << "Disk space trigger started.";
}
void DiskSpaceTrigger::stop()
{
    m_timer->stop();
    qDebug() << "Disk space trigger stopped.";
}
void DiskSpaceTrigger::checkDiskSpace()
{

    for(const auto &disk : m_disks){
        QStorageInfo storage = QStorageInfo::root();
        if(!disk.isEmpty()){
            for(const auto& storageInfo : QStorageInfo::mountedVolumes()){
                if(storageInfo.displayName() == disk){
                    storage = storageInfo;
                    break;
                }
            }
        }

        if(storage.isValid()){
            float freeSpace = storage.bytesFree();
            float totalSpace = storage.bytesTotal();
            float usedSpace = totalSpace - freeSpace;
            float usedPercentage = (usedSpace / totalSpace) * 100;
            if (usedPercentage > m_threshold) {
                qDebug() << "Disk " + storage.displayName() + " used space " + QString::number(usedPercentage) + "% is above threshold!";
            } else {
                qDebug() << "Disk " + storage.displayName() + " used space " + QString::number(usedPercentage) + "% is below threshold.";
            }
        }else{
            qDebug() << "Disk " + disk + " not found!";
        }
    }
}

QStringList DiskSpaceTrigger::disks() const
{
    return m_disks;
}

int DiskSpaceTrigger::threshold() const
{
    return m_threshold;
}

int DiskSpaceTrigger::interval() const
{
    return m_interval;
}
