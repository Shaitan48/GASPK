#include "Task.h"
#include "Trigger.h"
#include "Operation.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>

Task::Task(qlonglong id, QObject *parent) : QObject(parent), m_id(id), m_enabled(true)
{
}
Task::~Task() {
    for (const auto& trigger : m_triggers) {
        delete trigger;
    }
    for(const auto &operation : m_operations){
        delete operation;
    }
}

qlonglong Task::id() const {
    return m_id;
}

bool Task::isEnabled() const {
    return m_enabled;
}

void Task::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (m_enabled){
        for (const auto& trigger : m_triggers) {
            trigger->start();
        }
    } else{
        for (const auto& trigger : m_triggers) {
            trigger->stop();
        }
    }
}

void Task::addTrigger(Trigger* trigger)
{
    m_triggers.append(trigger);
    if (m_enabled){
        trigger->start();
    }
}
void Task::addOperation(Operation* operation)
{
    m_operations.append(operation);
}
QList<Trigger*> Task::getTriggers() const {
    return m_triggers;
}
QList<Operation*> Task::getOperations() const {
    return m_operations;
}
QJsonObject Task::toJson() const {
    QJsonObject json;
    json["id"] = (qlonglong)m_id;
    json["enabled"] = m_enabled;
    return json;
}
