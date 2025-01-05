#include "DiskSpaceTrigger.h"
#include <QStorageInfo>
#include <QDebug>
#include <QTimer>
#include <QJsonObject>
DiskSpaceTrigger::DiskSpaceTrigger(const QStringList &disks, int threshold, int interval, QObject *parent)
    : Trigger(parent), disks(disks), threshold(threshold), interval(interval), timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &DiskSpaceTrigger::checkDiskSpace);
}
DiskSpaceTrigger::~DiskSpaceTrigger(){
    stop();
}
void DiskSpaceTrigger::start()
{
    if (timer && !timer->isActive()) {
        timer->start(interval);
        checkDiskSpace();
    }
}
void DiskSpaceTrigger::stop() {
    if (timer && timer->isActive()) {
        timer->stop();
    }
}
void DiskSpaceTrigger::checkDiskSpace()
{
    bool anyDiskBelowThreshold = false;
    QJsonObject result;
    for (const QString &disk : disks)
    {
        QStorageInfo storage(disk);
        if (storage.isValid() )
        {
            qint64 freeSpace = storage.bytesAvailable() / (1024 * 1024);
            if (freeSpace < threshold)
            {
                qDebug() << "Disk space on " << disk << " is below threshold: " << freeSpace << " MB";
                anyDiskBelowThreshold = true;
            }else {
                qDebug() << "Disk space on " << disk << ": " << freeSpace << " MB";
            }
            result[disk] = static_cast<int>(freeSpace);
        }else {
            qDebug() << "Disk " << disk << " is not valid";
        }

    }
    result["anyDiskBelowThreshold"] = anyDiskBelowThreshold;
    emit triggered(result);
}
