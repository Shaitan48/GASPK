#ifndef TRIGGER_H
#define TRIGGER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>
class Task;

class Trigger : public QObject
{
    Q_OBJECT
public:
    explicit Trigger(qlonglong taskId, Task* task, QObject *parent = nullptr);
    virtual ~Trigger() = default;

    qlonglong id() const;
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void stateChanged(const QJsonObject& taskStateChange, QTcpSocket* client);

protected:
    Task* m_task;
private:
    qlonglong m_id;
};

#endif // TRIGGER_H
