#include "PingTrigger.h"
#include <QDebug>
#include <QTimer>
#include "Task.h"
#include <QJsonObject>

PingTrigger::PingTrigger(qlonglong taskId, Task* task, const QString& target, int interval, QObject* parent)
    : Trigger(taskId, task, parent), m_target(target), m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &PingTrigger::onTimer);
}

QString PingTrigger::getTarget() const { return m_target; }

int PingTrigger::getInterval() const { return m_interval; }


void PingTrigger::start() {
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(m_interval);
        qDebug() << "PingTrigger " << id() << " started.";
    }
}

void PingTrigger::stop() {
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "PingTrigger " << id() << " stopped.";
    }
}

void PingTrigger::onTimer()
{
    if(!m_task->isEnabled()) return;

    // Выполните пинг, если нужно
    QJsonObject data;
    data["id"] = (qlonglong)id();
    data["enabled"] = false;
    emit stateChanged(data, nullptr);
    qDebug() << "Ping " + m_target;
}
