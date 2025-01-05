#include "PingTrigger.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QJsonObject>

PingTrigger::PingTrigger(const QString &target, int interval, QObject *parent) : Trigger(parent), target(target), interval(interval), timer(new QTimer(this)) {
    connect(timer, &QTimer::timeout, this, &PingTrigger::ping);
}
PingTrigger::~PingTrigger(){
    stop();
}
void PingTrigger::start() {
    if (timer && !timer->isActive()) {
        timer->start(interval);
        ping();
    }
}

void PingTrigger::stop() {
    if (timer && timer->isActive()) {
        timer->stop();
    }
}
void PingTrigger::ping() {
    QProcess *process = new QProcess(this);
#ifdef Q_OS_WIN
    QStringList args = {"-n", "1", target};
    process->start("ping", args);
#else
    QStringList args = {"-c", "1", target};
    process->start("ping", args);
#endif
    process->waitForFinished();
    QJsonObject result;
    if (process->exitCode() == 0) {
        qDebug() << "Ping to " << target << " successful.";
        result["success"] = true;

    } else {
        qDebug() << "Ping to " << target << " failed.";
        result["success"] = false;
    }
    emit triggered(result);
    process->deleteLater();
}
