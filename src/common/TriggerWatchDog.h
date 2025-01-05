#ifndef TRIGGERWATCHDOG_H
#define TRIGGERWATCHDOG_H

#include "Trigger.h"
#include <QString>
#include <QTimer>
class Task;

class TriggerWatchDog : public Trigger
{
    Q_OBJECT
public:
    TriggerWatchDog(qlonglong taskId, Task* task, const QString& path, const QString& mask, int interval, QObject* parent = nullptr);
    ~TriggerWatchDog() override;

    QString getPath() const;
    QString getMask() const;
    int getInterval() const;

    void start() override;
    void stop() override;

private slots:
    void onTimer();

private:
    QString m_path;
    QString m_mask;
    int m_interval;
    QTimer* m_timer;
};

#endif // TRIGGERWATCHDOG_H
