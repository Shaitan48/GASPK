#include "DiskSpaceTrigger.h"
#include <QDebug>
#include <QStorageInfo>
#include <QTimer>
#include <QJsonObject>
#include "Task.h"

DiskSpaceTrigger::DiskSpaceTrigger(qlonglong taskId, Task* task, const QStringList& disks, int threshold, int interval, QObject* parent)
    : Trigger(taskId, task, parent), m_disks(disks), m_threshold(threshold), m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &DiskSpaceTrigger::onTimer);
}

DiskSpaceTrigger::~DiskSpaceTrigger() {
    stop();
}

QStringList DiskSpaceTrigger::getDisks() const {
    return m_disks;
}

int DiskSpaceTrigger::getThreshold() const {
    return m_threshold;
}

int DiskSpaceTrigger::getInterval() const {
    return m_interval;
}

void DiskSpaceTrigger::start() {
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(m_interval);
        qDebug() << "DiskSpaceTrigger " << id() << " started.";
    }
}

void DiskSpaceTrigger::stop() {
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "DiskSpaceTrigger " << id() << " stopped.";
    }
}

void DiskSpaceTrigger::onTimer()
{
    if(!m_task->isEnabled()) return;
    for (const QString &disk : m_disks) {
        QStorageInfo storage(disk);
        if (storage.isValid()) {
            qlonglong freeSpace = storage.bytesFree() / (1024 * 1024); // Free space in MB
            qlonglong totalSpace = storage.bytesTotal() / (1024 * 1024);
            qlonglong usedSpace = totalSpace - freeSpace;
            if (usedSpace > m_threshold){
                QJsonObject data;
                data["id"] = (qlonglong)id();
                data["enabled"] = false;
                emit stateChanged(data, nullptr);
            }
            qDebug() << "Disk " + disk + ": Total " + QString::number(totalSpace) + " MB, Used: " + QString::number(usedSpace) + " MB, Free " + QString::number(freeSpace) + " MB";
        } else {
            qDebug() << "Invalid disk " + disk;
        }
    }
}
