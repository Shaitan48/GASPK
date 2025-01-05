#ifndef DISKSPACETRIGGER_H
#define DISKSPACETRIGGER_H

#include "Trigger.h"
#include <QStringList>
#include <QObject>
#include <QTimer>

class DiskSpaceTrigger : public Trigger
{
    Q_OBJECT
public:
    DiskSpaceTrigger(const QStringList &disks, int threshold, int interval, QObject *parent = nullptr);
    ~DiskSpaceTrigger() override;
    void start() override;
    void stop() override;

private:
    void checkDiskSpace();
    QStringList disks;
    int threshold;
    int interval;
    QTimer *timer;
};

#endif // DISKSPACETRIGGER_H
