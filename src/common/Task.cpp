#include "Task.h"
#include <QDebug>
Task::Task(QObject *parent) : QObject(parent)
{
}

void Task::addTrigger(Trigger *trigger)
{
    triggers.append(trigger);
}

void Task::addOperation(Operation *operation)
{
    operations.append(operation);
}

void Task::process(const QJsonObject &agentData)
{
    for (Trigger* trigger : triggers) {
        if (trigger->isTriggered(agentData)) {
            qDebug() << "Task: Triggered";
            for (Operation *operation : operations) {
                operation->execute(agentData);
            }
            return;
        }
    }
}

Task::~Task()
{
    for (Trigger* trigger : triggers) {
        delete trigger;
    }
    for(Operation* operation: operations){
        delete operation;
    }
}
