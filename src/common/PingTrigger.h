#ifndef PINGTRIGGER_H
#define PINGTRIGGER_H

#include "Trigger.h"
#include <QString>
#include <QTimer>

class PingTrigger : public Trigger
{
    Q_OBJECT
public:
    PingTrigger(const QString &target, int interval, QObject *parent = nullptr);
    ~PingTrigger() override;
    void start() override;
    void stop() override;

private:
    void ping();
    QString target;
    int interval;
    QTimer *timer;

};

#endif // PINGTRIGGER_H
