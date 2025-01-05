#include "Agent.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "PingTrigger.h"
#include "DiskSpaceTrigger.h"
#include "TriggerWatchDog.h"
#include "Task.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>

Agent::Agent(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this)), manager(new QNetworkAccessManager(this))
{
    connect(tcpSocket, &QTcpSocket::connected, this, &Agent::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Agent::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Agent::onDisconnected);
    connect(manager, &QNetworkAccessManager::finished, this, &Agent::onReply);

    loadConfig();

    qDebug() << "Agent started.";
}
void Agent::loadConfig()
{
    QFile file(QDir::currentPath() + "/config.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open config file " + file.fileName() + " " + file.errorString();
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "Could not parse json from config file " + file.fileName() + " " + jsonError.errorString();
        return;
    }
    if(jsonDoc.isObject()){
        QJsonObject config = jsonDoc.object();
        if (config.contains("server") && config["server"].isObject()) {
            QJsonObject serverConfig = config["server"].toObject();
            if(serverConfig.contains("host") && serverConfig.contains("port")){
                QString host = serverConfig["host"].toString();
                int port = serverConfig["port"].toInt();
                tcpSocket->connectToHost(host, port);
            }
        }
    }
}
void Agent::onConnected() {
    qDebug() << "Connected to server.";
    sendSystemInfo();
}
void Agent::onReadyRead() {
    QByteArray data = tcpSocket->readAll();
    qDebug() << "Data from server: " << data;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if(jsonDoc.isObject()){
            QJsonObject json = jsonDoc.object();
            if (json.contains("tasks") && json["tasks"].isArray()){
                QJsonArray tasksArray = json["tasks"].toArray();
                for (const auto& taskValue : tasksArray){
                    if(taskValue.isObject()){
                        QJsonObject taskJson = taskValue.toObject();
                        if(taskJson.contains("id")){
                            qlonglong id = taskJson["id"].toVariant().toLongLong();
                            Task* task = new Task(id, this);

                            if(taskJson.contains("ping") && taskJson["ping"].isArray()){
                                QJsonArray pingArray = taskJson["ping"].toArray();
                                for(const auto &pingValue : pingArray){
                                    if(pingValue.isObject()){
                                        QJsonObject pingConfig = pingValue.toObject();
                                        if(pingConfig.contains("target") && pingConfig.contains("interval")) {
                                            QString target = pingConfig["target"].toString();
                                            int interval = pingConfig["interval"].toInt();
                                            PingTrigger *trigger = new PingTrigger(id,task, target, interval, this);
                                            task->addTrigger(trigger);
                                        }
                                    }
                                }
                            }
                            if(taskJson.contains("diskSpace") && taskJson["diskSpace"].isArray()){
                                QJsonArray diskSpaceArray = taskJson["diskSpace"].toArray();
                                for(const auto &diskSpaceValue : diskSpaceArray){
                                    if(diskSpaceValue.isObject()){
                                        QJsonObject diskSpaceConfig = diskSpaceValue.toObject();
                                        if(diskSpaceConfig.contains("disks") && diskSpaceConfig.contains("threshold") && diskSpaceConfig.contains("interval")) {
                                            QStringList disks;
                                            if(diskSpaceConfig["disks"].isArray()){
                                                QJsonArray disksArray = diskSpaceConfig["disks"].toArray();
                                                for(const auto &diskValue : disksArray){
                                                    if(diskValue.isString()){
                                                        disks.append(diskValue.toString());
                                                    }
                                                }
                                            }
                                            int threshold = diskSpaceConfig["threshold"].toInt();
                                            int interval = diskSpaceConfig["interval"].toInt();
                                            DiskSpaceTrigger *trigger = new DiskSpaceTrigger(id, task,disks, threshold, interval, this);
                                            task->addTrigger(trigger);
                                        }
                                    }
                                }
                            }
                            if(taskJson.contains("watchDog") && taskJson["watchDog"].isArray()){
                                QJsonArray watchDogArray = taskJson["watchDog"].toArray();
                                for(const auto &watchDogValue : watchDogArray){
                                    if(watchDogValue.isObject()){
                                        QJsonObject watchDogConfig = watchDogValue.toObject();
                                        if (watchDogConfig.contains("path") && watchDogConfig.contains("mask") && watchDogConfig.contains("interval")) {
                                            QString path = watchDogConfig["path"].toString();
                                            QString mask = watchDogConfig["mask"].toString();
                                            int interval = watchDogConfig["interval"].toInt();
                                            TriggerWatchDog *trigger = new TriggerWatchDog(id, task,path, mask, interval, this);
                                            task->addTrigger(trigger);
                                        }
                                    }
                                }
                            }
                            tasks.append(task);
                        }
                    }

                }
            }

        }
    }
}
void Agent::onDisconnected() {
    qDebug() << "Disconnected from server.";
}
void Agent::sendSystemInfo() {
    QJsonObject systemInfo;
    systemInfo["hostname"] = QHostInfo::localHostName();
    systemInfo["username"] = QString::fromUtf8(qgetenv("USERNAME"));  // Преобразуем QByteArray в QString
    QJsonDocument jsonDoc(systemInfo);
    tcpSocket->write(jsonDoc.toJson());
    qDebug() << "Sent system info to server: " << jsonDoc.toJson();
}
void Agent::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response: " << response;
        if (reply->url().toString() == "http://localhost:1234/taskStateChanged") {
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &jsonError);
            if (jsonError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) {
                    QJsonObject taskStateChange = jsonDoc.object();

                    // Correctly handle the case where id or enabled are missing.
                    if (taskStateChange.contains("id") && taskStateChange.contains("enabled")) {
                        qlonglong id = taskStateChange["id"].toVariant().toLongLong();
                        bool enabled = taskStateChange["enabled"].toBool();
                        // Find the task, and update the enabled state. Crucial change!
                        Task *task = findTaskById(id);
                        if (task) {
                            task->setEnabled(enabled);
                            qDebug() << "Task " + QString::number(id) + " state changed. Enabled: " << enabled;
                        } else {
                            qDebug() << "Task with ID" << id << "not found!";
                        }

                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();
}
Task* Agent::findTaskById(qlonglong id)
{
    for (Task* task : tasks)
    {
        if (task->id() == id)
        {
            return task;
        }
    }
    return nullptr;
}
Agent::~Agent()
{
    for(const auto& task : tasks){
        for (const auto& trigger : task->getTriggers()){
            delete trigger;
        }
        delete task;
    }

}
