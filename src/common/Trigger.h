#ifndef TRIGGER_H
#define TRIGGER_H

#include <QObject>
#include <QJsonObject>

class Trigger : public QObject
{
    Q_OBJECT
public:
    explicit Trigger(QObject *parent = nullptr);
    virtual ~Trigger() = default;
    virtual void start();
    virtual void stop();
    virtual bool isTriggered(const QJsonObject &agentData) {
        Q_UNUSED(agentData);
        return false;
    };


signals:
    void triggered(const QJsonObject &result);

};

#endif // TRIGGER_H
