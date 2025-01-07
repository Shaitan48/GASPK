#ifndef PINGTRIGGER_H
#define PINGTRIGGER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "Trigger.h"

class PingTrigger : public Trigger
{
    Q_OBJECT
public:
    explicit PingTrigger(qlonglong id, QObject *parent = nullptr, const QString &target = "", int interval = 60);
    ~PingTrigger() override;
    void start() override;
    void stop() override;
    QString target() const;
    int interval() const;

private:
    QString m_target;
    int m_interval;
    QTimer *m_timer;
    // Удалите эту зависимость:
    // QObject *m_task;
};

#endif // PINGTRIGGER_H
