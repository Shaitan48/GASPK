#ifndef DISKSPACETRIGGER_H
#define DISKSPACETRIGGER_H

#include "Trigger.h"
#include <QStringList>
#include <QTimer>

class DiskSpaceTrigger : public Trigger
{
    Q_OBJECT
public:
    explicit DiskSpaceTrigger(qlonglong id, QObject *parent = nullptr,const QStringList& disks = {}, int threshold = 80, int interval = 60);
    ~DiskSpaceTrigger() override;
    void start() override;
    void stop() override;
    QStringList disks() const;
    int threshold() const;
    int interval() const;

private:
    QStringList m_disks;
    int m_threshold;
    int m_interval;
    QTimer *m_timer;

};

#endif // DISKSPACETRIGGER_H
