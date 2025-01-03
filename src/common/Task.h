#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QList>
#include "Trigger.h"
#include "Operation.h"

class Task : public QObject
{
    Q_OBJECT

public:
    explicit Task(QObject *parent = nullptr);
    void addTrigger(Trigger *trigger);
    void addOperation(Operation *operation);
    void process(const QJsonObject& agentData);
    ~Task() override;

private:
    QList<Trigger*> triggers;
    QList<Operation*> operations;
};

#endif // TASK_H
