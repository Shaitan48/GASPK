#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QList>
#include <QJsonObject>

class Trigger;
class Operation;
class Task : public QObject
{
    Q_OBJECT

public:
    explicit Task(qlonglong id, QObject *parent = nullptr);
    ~Task() override;
    void addTrigger(Trigger* trigger);
    void removeTrigger(Trigger* trigger);
    QList<Trigger*> getTriggers() const;
    qlonglong id() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    void addOperation(Operation* operation);
    void removeOperation(Operation* operation);
    QList<Operation*> getOperations() const;
    QJsonObject toJson() const;
    Q_SIGNAL void stateChanged(const QJsonObject& taskStateChange, QTcpSocket* client);
private:
    qlonglong _id;
    bool _enabled = true;
    QList<Trigger*> _triggers;
    QList<Operation*> _operations;
};

#endif // TASK_H
