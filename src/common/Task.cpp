#include "Task.h"
#include "Trigger.h"
#include "Operation.h"
#include <QDebug>
#include <QTcpSocket>
Task::Task(qlonglong id, QObject *parent)
    : QObject(parent), _id(id)
{
    qDebug() << "Task created with id " + QString::number(id);
}
Task::~Task()
{
    qDebug() << "Task with id " + QString::number(_id) + " deleted.";
    for(auto trigger: _triggers){
        delete trigger;
    }
    for(auto operation : _operations){
        delete operation;
    }
}
void Task::addTrigger(Trigger *trigger)
{
    _triggers.append(trigger);
}
void Task::removeTrigger(Trigger *trigger)
{
    _triggers.removeOne(trigger);
}
QList<Trigger *> Task::getTriggers() const
{
    return _triggers;
}
qlonglong Task::id() const{
    return _id;
}
bool Task::isEnabled() const
{
    return _enabled;
}
void Task::setEnabled(bool enabled)
{
    if(enabled != _enabled){
        _enabled = enabled;
        QJsonObject taskStateChange;
        taskStateChange["id"] = (qlonglong)id();
        taskStateChange["enabled"] = isEnabled();
        emit stateChanged(taskStateChange, nullptr);
        qDebug() << "Task " + QString::number(_id) + " state changed. Enabled: " << enabled;
        for(const auto& trigger : _triggers){
            if (enabled){
                trigger->start();
            } else {
                trigger->stop();
            }
        }
    }
}
void Task::addOperation(Operation *operation)
{
    _operations.append(operation);
}
void Task::removeOperation(Operation *operation)
{
    _operations.removeOne(operation);
}
QList<Operation *> Task::getOperations() const
{
    return _operations;
}

QJsonObject Task::toJson() const
{
    QJsonObject taskJson;
    taskJson["id"] = (qlonglong)_id;
    taskJson["enabled"] = _enabled;
    return taskJson;
}
