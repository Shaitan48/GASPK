#ifndef TRIGGER_H
#define TRIGGER_H

#include <QObject>
#include <QJsonObject>

class Trigger : public QObject
{
    Q_OBJECT

public:
    explicit Trigger(QObject *parent = nullptr);
    virtual bool isTriggered(const QJsonObject& agentData) = 0;
    virtual ~Trigger() = default;

signals:
    void triggered(const QJsonObject& agentData);
};

#endif // TRIGGER_H
