#include "PingTrigger.h"
#include <QDebug>
#include <QProcess>


PingTrigger::PingTrigger(qlonglong id, QObject *parent, const QString &target, int interval) : Trigger(id, parent),
    m_target(target), m_interval(interval), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &PingTrigger::pingTarget);
    qDebug() << "Ping trigger created. Target: " + target + ", interval " + QString::number(interval);
}


PingTrigger::~PingTrigger()
{
    m_timer->stop();
    delete m_timer;
    qDebug() << "Ping trigger deleted.";
}

void PingTrigger::start()
{
    m_timer->start(m_interval * 1000);
    qDebug() << "Ping trigger started.";
}

void PingTrigger::stop()
{
    m_timer->stop();
    qDebug() << "Ping trigger stopped.";
}

void PingTrigger::pingTarget()
{
    QProcess *process = new QProcess();
    QStringList arguments;
    arguments << "-n" << "1" << m_target;
    process->start("ping", arguments);
    connect(process, &QProcess::finished, this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            qDebug() << "Target " + m_target + " is reachable";
        } else {
            qDebug() << "Target " + m_target + " is not reachable";
        }
        process->deleteLater();
    });
}

QString PingTrigger::target() const
{
    return m_target;
}

int PingTrigger::interval() const
{
    return m_interval;
}
