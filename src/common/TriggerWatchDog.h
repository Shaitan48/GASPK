#ifndef TRIGGERWATCHDOG_H
#define TRIGGERWATCHDOG_H

#include "Trigger.h"
#include <QString>
#include <QTimer>

class TriggerWatchDog : public Trigger
{
    Q_OBJECT
public:
    explicit TriggerWatchDog(qlonglong id, QObject *parent = nullptr, const QString& path = "", const QString& mask = "", int interval = 60);
    ~TriggerWatchDog() override;
    void start() override;
    void stop() override;
    QString path() const;
    QString mask() const;
    int interval() const;
private:
    QString m_path;
    QString m_mask;
    int m_interval;
    QTimer *m_timer;
};

#endif // TRIGGERWATCHDOG_H
