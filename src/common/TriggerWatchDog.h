#ifndef TRIGGERWATCHDOG_H
#define TRIGGERWATCHDOG_H

#include "Trigger.h"
#include <QString>
#include <QObject>
#include <QTimer>

class TriggerWatchDog : public Trigger
{
    Q_OBJECT
public:
    TriggerWatchDog(const QString &path, const QString &mask, int interval, QObject *parent = nullptr);
    ~TriggerWatchDog() override;
    void start() override;
    void stop() override;

private:
    void checkDir();
    QString path;
    QString mask;
    int interval;
    QTimer *timer;
public:
    bool isTriggered(const QJsonObject &agentData) override;
};

#endif // TRIGGERWATCHDOG_H
