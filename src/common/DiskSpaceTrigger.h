#ifndef DISKSPACETRIGGER_H
#define DISKSPACETRIGGER_H

#include "Trigger.h"
#include <QStringList>
#include <QTimer>
class Task;

class DiskSpaceTrigger : public Trigger
{
    Q_OBJECT
public:
    DiskSpaceTrigger(qlonglong taskId, Task* task, const QStringList& disks, int threshold, int interval, QObject* parent = nullptr);
    ~DiskSpaceTrigger() override;

    QStringList getDisks() const;
    int getThreshold() const;
    int getInterval() const;

    void start() override;
    void stop() override;

private slots:
    void onTimer();

private:
    QStringList m_disks;
    int m_threshold;
    int m_interval;
    QTimer* m_timer;
};

#endif // DISKSPACETRIGGER_H
