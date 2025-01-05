#ifndef PINGTRIGGER_H
#define PINGTRIGGER_H

#include "Trigger.h"
#include <QString>
#include <QTimer>
class Task;
class PingTrigger : public Trigger
{
    Q_OBJECT
public:
    explicit PingTrigger(qlonglong taskId, Task* task, const QString& target, int interval, QObject* parent = nullptr);
    ~PingTrigger() override = default;

    QString getTarget() const;
    int getInterval() const;

    void start() override;
    void stop() override;
private slots:
    void onTimer();

private:
    QString m_target;
    int m_interval;
    QTimer* m_timer;
};

#endif // PINGTRIGGER_H
