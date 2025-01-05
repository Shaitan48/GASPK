#include "Trigger.h"
#include <QDebug>
#include "Task.h"

Trigger::Trigger(qlonglong taskId, Task* task, QObject *parent) : QObject(parent), m_id(taskId), m_task(task)
{
}
qlonglong Trigger::id() const {
    return m_id;
}
void Trigger::start(){
    qDebug() << "Trigger start method called with id " << id();
}
void Trigger::stop(){
    qDebug() << "Trigger stop method called with id " << id();
}
