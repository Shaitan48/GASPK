#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QList>
#include "Trigger.h"
#include "Operation.h"
#include <QJsonObject>
class Trigger;
class Operation;
class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(qlonglong id, QObject *parent = nullptr);
    ~Task() override;
    qlonglong id() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    void addTrigger(Trigger* trigger);
    void addOperation(Operation* operation);
    QList<Trigger*> getTriggers() const;
    QList<Operation*> getOperations() const;
    QJsonObject toJson() const;
signals:
    void stateChanged(const QJsonObject& taskStateChange, QTcpSocket* client);

private:
    qlonglong m_id;
    bool m_enabled;
    QList<Trigger*> m_triggers;
    QList<Operation*> m_operations;
};

#endif // TASK_H
