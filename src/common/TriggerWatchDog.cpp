#include "TriggerWatchDog.h"
#include <QDirIterator>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
TriggerWatchDog::TriggerWatchDog(const QString &path, const QString &mask,  int interval, QObject *parent)
    : Trigger(parent), path(path), mask(mask), interval(interval), timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &TriggerWatchDog::checkDir);
}
TriggerWatchDog::~TriggerWatchDog(){
    stop();
}
void TriggerWatchDog::start()
{
    if (timer && !timer->isActive()) {
        timer->start(interval);
        checkDir();
    }
}
void TriggerWatchDog::stop()
{
    if (timer && timer->isActive()) {
        timer->stop();
    }
}

void TriggerWatchDog::checkDir()
{
    QDirIterator it(path, QDirIterator::Subdirectories);
    QJsonObject data;
    bool found = false;
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        if(fileInfo.isFile()){
            if(fileInfo.fileName().contains(mask, Qt::CaseInsensitive)){
                qDebug() << "File detected: " << filePath;
                data["filePath"] = filePath;
                data["datetime"] =  QDateTime::currentDateTime().toString();
                found = true;
                break;
            }
        }
    }

    emit triggered(data);
}
bool TriggerWatchDog::isTriggered(const QJsonObject &agentData){
    Q_UNUSED(agentData);
    return true;
}
