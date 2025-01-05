#include "TriggerWatchDog.h"
#include <QDebug>
#include <QFileSystemWatcher>
#include <QDir>
#include <QTimer>
#include <QJsonObject>
#include "Task.h"

TriggerWatchDog::TriggerWatchDog(qlonglong taskId, Task* task, const QString& path, const QString& mask, int interval, QObject* parent)
    : Trigger(taskId, task, parent), m_path(path), m_mask(mask), m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &TriggerWatchDog::onTimer);
}
TriggerWatchDog::~TriggerWatchDog() {
    stop();
}

QString TriggerWatchDog::getPath() const {
    return m_path;
}

QString TriggerWatchDog::getMask() const {
    return m_mask;
}

int TriggerWatchDog::getInterval() const {
    return m_interval;
}

void TriggerWatchDog::start() {
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(m_interval);
        qDebug() << "TriggerWatchDog " << id() << " started.";
    }
}

void TriggerWatchDog::stop() {
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "TriggerWatchDog " << id() << " stopped.";
    }
}

void TriggerWatchDog::onTimer() {
    if(!m_task->isEnabled()) return;
    QDir dir(m_path);
    if (!dir.exists()) {
        qDebug() << "Path not exists" + m_path;
        return;
    }

    QStringList files = dir.entryList(QStringList() << m_mask, QDir::Files);
    if(!files.isEmpty()){
        QJsonObject data;
        data["id"] = (qlonglong)id();
        data["enabled"] = false;
        emit stateChanged(data, nullptr);
    }

}
