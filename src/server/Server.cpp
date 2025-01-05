#include "Server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QJsonArray>
#include <QDir>
#include "Trigger.h"
#include "Operation.h"
#include "PingTrigger.h"
#include "DiskSpaceTrigger.h"
#include "TriggerWatchDog.h"
#include "Task.h"
#include "FileDeleteOperation.h"
#include "FileCopyOperation.h"
#include "FileMoveOperation.h"
#include "MessageOperation.h"
#include "RequestOperation.h"
#include <QCoreApplication>

Server::Server(QObject *parent) : QObject(parent), tcpServer(new QTcpServer(this)), manager(new QNetworkAccessManager(this))
{
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    connect(manager, &QNetworkAccessManager::finished, this, &Server::onReply);
    loadTasksFromFile();
    qDebug() << "Server started.";
}
void Server::loadTasksFromFile(){
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
                if(!tcpServer->listen(QHostAddress(host), port)) {
                    qDebug() << "Error: " << tcpServer->errorString();
                    QCoreApplication::exit(1);
                }
            }
        }
        if (config.contains("database") && config["database"].isObject()){
            QJsonObject databaseConfig = config["database"].toObject();
            if(databaseConfig.contains("type") && databaseConfig.contains("path")) {
                QString type = databaseConfig["type"].toString();
                QString path = databaseConfig["path"].toString();
                // Логика подключения к БД в зависимости от типа
                qDebug() << "Database: type " + type + " path " + path;
            }

        }
        if (config.contains("tasks") && config["tasks"].isArray()){
            QJsonArray tasksArray = config["tasks"].toArray();
            for(const auto &taskValue : tasksArray){
                if (taskValue.isObject()) {
                    QJsonObject taskJson = taskValue.toObject();
                    if (taskJson.contains("id") && taskJson.contains("enabled")){
                        qlonglong id = taskJson["id"].toVariant().toLongLong();
                        bool enabled = taskJson["enabled"].toBool(true);
                        Task* task = new Task(id, this);
                        connect(task, &Task::stateChanged, this, &Server::handleTaskStateChanged);
                        task->setEnabled(enabled);
                        if(taskJson.contains("ping") && taskJson["ping"].isObject()){
                            QJsonObject pingConfig = taskJson["ping"].toObject();
                            if (pingConfig.contains("target") && pingConfig.contains("interval")) {
                                QString target = pingConfig["target"].toString();
                                int interval = pingConfig["interval"].toInt();
                                PingTrigger* trigger = new PingTrigger(id,task,target, interval, this);
                                task->addTrigger(trigger);
                            }
                        }

                        if(taskJson.contains("diskSpace") && taskJson["diskSpace"].isObject()){
                            QJsonObject diskSpaceConfig = taskJson["diskSpace"].toObject();
                            if (diskSpaceConfig.contains("disks") && diskSpaceConfig.contains("threshold") && diskSpaceConfig.contains("interval")) {
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
                                DiskSpaceTrigger* trigger = new DiskSpaceTrigger(id, task, disks, threshold, interval, this);
                                task->addTrigger(trigger);
                            }
                        }
                        if(taskJson.contains("watchDog") && taskJson["watchDog"].isObject()){
                            QJsonObject watchDogConfig = taskJson["watchDog"].toObject();
                            if (watchDogConfig.contains("path") && watchDogConfig.contains("mask") && watchDogConfig.contains("interval")) {
                                QString path = watchDogConfig["path"].toString();
                                QString mask = watchDogConfig["mask"].toString();
                                int interval = watchDogConfig["interval"].toInt();
                                TriggerWatchDog* trigger = new TriggerWatchDog(id, task, path, mask, interval, this);
                                task->addTrigger(trigger);
                            }
                        }
                        if (taskJson.contains("operations") && taskJson["operations"].isArray()){
                            QJsonArray operationsArray = taskJson["operations"].toArray();
                            for(const auto &operationValue : operationsArray) {
                                if(operationValue.isObject()){
                                    QJsonObject operationJson = operationValue.toObject();
                                    if(operationJson.contains("name")){
                                        QString operationName = operationJson["name"].toString();
                                        qlonglong operationId = operationJson["id"].toVariant().toLongLong();
                                        if(operationName == "fileDelete") {
                                            FileDeleteOperation* operation = new FileDeleteOperation(operationId,operationJson["parameters"].toObject(),this);
                                            task->addOperation(operation);
                                        } else if (operationName == "fileCopy"){
                                            FileCopyOperation* operation = new FileCopyOperation(operationId,operationJson["parameters"].toObject(),this);
                                            task->addOperation(operation);
                                        } else if (operationName == "fileMove"){
                                            FileMoveOperation* operation = new FileMoveOperation(operationId,operationJson["parameters"].toObject(),this);
                                            task->addOperation(operation);
                                        } else if (operationName == "message"){
                                            MessageOperation* operation = new MessageOperation(operationId,operationJson["parameters"].toObject(),this);
                                            task->addOperation(operation);
                                        } else if (operationName == "request"){
                                            RequestOperation* operation = new RequestOperation(operationId,operationJson["parameters"].toObject(),this);
                                            task->addOperation(operation);
                                        }
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

void Server::saveTasksToFile() const{
    QJsonArray tasksArray;
    for (const auto& task : tasks){
        QJsonObject taskJson = task->toJson();
        QJsonArray triggersArray;
        for(const auto& trigger : task->getTriggers()){
            QJsonObject triggerJson;
            if(dynamic_cast<PingTrigger*>(trigger)) {
                triggerJson["target"] = dynamic_cast<PingTrigger*>(trigger)->getTarget();
                triggerJson["interval"] = dynamic_cast<PingTrigger*>(trigger)->getInterval();
                taskJson["ping"] = triggerJson;
            }
            else if(dynamic_cast<DiskSpaceTrigger*>(trigger)) {
                triggerJson["disks"] =  QJsonArray::fromStringList(dynamic_cast<DiskSpaceTrigger*>(trigger)->getDisks());
                triggerJson["threshold"] =  dynamic_cast<DiskSpaceTrigger*>(trigger)->getThreshold();
                triggerJson["interval"] =  dynamic_cast<DiskSpaceTrigger*>(trigger)->getInterval();
                taskJson["diskSpace"] = triggerJson;
            }
            else if(dynamic_cast<TriggerWatchDog*>(trigger)) {
                triggerJson["path"] =  dynamic_cast<TriggerWatchDog*>(trigger)->getPath();
                triggerJson["mask"] =  dynamic_cast<TriggerWatchDog*>(trigger)->getMask();
                triggerJson["interval"] =  dynamic_cast<TriggerWatchDog*>(trigger)->getInterval();
                taskJson["watchDog"] = triggerJson;
            }
        }
        QJsonArray operationsArray;
        for (const auto& operation : task->getOperations()){
            QJsonObject operationJson;
            if (dynamic_cast<FileDeleteOperation*>(operation)){
                operationJson["name"] = "fileDelete";
                operationJson["parameters"] = dynamic_cast<FileDeleteOperation*>(operation)->parameters();
                operationsArray.append(operationJson);
            } else if (dynamic_cast<FileCopyOperation*>(operation)){
                operationJson["name"] = "fileCopy";
                operationJson["parameters"] = dynamic_cast<FileCopyOperation*>(operation)->parameters();
                operationsArray.append(operationJson);
            } else if (dynamic_cast<FileMoveOperation*>(operation)){
                operationJson["name"] = "fileMove";
                operationJson["parameters"] = dynamic_cast<FileMoveOperation*>(operation)->parameters();
                operationsArray.append(operationJson);
            }else if (dynamic_cast<MessageOperation*>(operation)){
                operationJson["name"] = "message";
                operationJson["parameters"] = dynamic_cast<MessageOperation*>(operation)->parameters();
                operationsArray.append(operationJson);
            }else if (dynamic_cast<RequestOperation*>(operation)){
                operationJson["name"] = "request";
                operationJson["parameters"] = dynamic_cast<RequestOperation*>(operation)->parameters();
                operationsArray.append(operationJson);
            }
        }
        if(operationsArray.size() > 0){
            taskJson["operations"] = operationsArray;
        }

        tasksArray.append(taskJson);
    }
    QJsonDocument jsonDoc(tasksArray);
    QFile file(QDir::currentPath() + "/config.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open config file " + file.fileName() + " " + file.errorString();
        return;
    }
    file.write(jsonDoc.toJson());
    file.close();
    qDebug() << "Config saved to file";
}
void Server::onNewConnection() {
    QTcpSocket* client = tcpServer->nextPendingConnection();
    if(client) {
        qDebug() << "New connection from " << client->peerAddress().toString();
        connect(client, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &Server::onDisconnected);
        clients[client] = "client";
    }
}
void Server::onReadyRead() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
        if (jsonError.error == QJsonParseError::NoError){
            if(jsonDoc.isObject()){
                QJsonObject json = jsonDoc.object();
                if(json.contains("hostname") && json.contains("username")){
                    qDebug() << "System info from agent " << client->peerAddress().toString() << ":" << data;
                    clients[client] = "agent";
                    sendSystemInfoToClient(client);
                } else if (json.contains("message")){
                    qDebug() << "Message from agent " << client->peerAddress().toString() << ":" << json["message"].toString();
                }
            }
        } else{
            qDebug() << "Data from " << client->peerAddress().toString() << ":" << data;
        }
        if(clients[client] == "client"){
            sendOperationsToClient(client);
        } else if (clients[client] == "agent"){
            sendTriggersToAgent(client);
        }
    }
}
void Server::sendTriggersToAgent(QTcpSocket* agent) {
    QJsonObject triggers;
    QJsonArray tasksArray;
    for (const auto& task : tasks){
        QJsonObject taskJson;
        taskJson["id"] = (qlonglong)task->id();
        QJsonArray pingArray;
        QJsonArray diskSpaceArray;
        QJsonArray watchDogArray;
        for(const auto& trigger : task->getTriggers()){
            if(dynamic_cast<PingTrigger*>(trigger)) {
                QJsonObject pingConfig;
                pingConfig["target"] = dynamic_cast<PingTrigger*>(trigger)->getTarget();
                pingConfig["interval"] = dynamic_cast<PingTrigger*>(trigger)->getInterval();
                pingArray.append(pingConfig);
            }
            if(dynamic_cast<DiskSpaceTrigger*>(trigger)) {
                QJsonObject diskSpaceConfig;
                diskSpaceConfig["disks"] = QJsonArray::fromStringList(dynamic_cast<DiskSpaceTrigger*>(trigger)->getDisks());
                diskSpaceConfig["threshold"] = dynamic_cast<DiskSpaceTrigger*>(trigger)->getThreshold();
                diskSpaceConfig["interval"] = dynamic_cast<DiskSpaceTrigger*>(trigger)->getInterval();
                diskSpaceArray.append(diskSpaceConfig);

            }
            if(dynamic_cast<TriggerWatchDog*>(trigger)){
                QJsonObject watchDogConfig;
                watchDogConfig["path"] = dynamic_cast<TriggerWatchDog*>(trigger)->getPath();
                watchDogConfig["mask"] = dynamic_cast<TriggerWatchDog*>(trigger)->getMask();
                watchDogConfig["interval"] = dynamic_cast<TriggerWatchDog*>(trigger)->getInterval();
                watchDogArray.append(watchDogConfig);
            }
        }
        taskJson["ping"] = pingArray;
        taskJson["diskSpace"] = diskSpaceArray;
        taskJson["watchDog"] = watchDogArray;
        tasksArray.append(taskJson);
    }

    triggers["tasks"] = tasksArray;

    QJsonDocument jsonDoc(triggers);
    agent->write(jsonDoc.toJson());
    qDebug() << "Sent triggers to agent " + agent->peerAddress().toString() + " data: " << jsonDoc.toJson();
}
void Server::handleTaskStateChanged(const QJsonObject & taskStateChange, QTcpSocket* client) {
    if(taskStateChange.contains("id") && taskStateChange.contains("enabled")){
        qlonglong id = taskStateChange["id"].toVariant().toLongLong();
        bool enabled = taskStateChange["enabled"].toBool();
        for (const auto& task : tasks) {
            if (task->id() == id){
                task->setEnabled(enabled);
                qDebug() << "Task " + QString::number(id) + " state changed. Enabled: " << enabled;
            }
        }
        saveTasksToFile();
    }

}
void Server::sendOperationsToClient(QTcpSocket* client) {
    QJsonArray operationsArray;

    for (const auto& task : tasks) {
        for(const auto& operation : task->getOperations()){
            QJsonObject operationJson;
            operationJson["id"] = (qlonglong)operation->id();
            operationJson["name"] = operation->name();
            operationJson["enabled"] = task->isEnabled();
            operationsArray.append(operationJson);
        }
    }
    QJsonDocument jsonDoc(operationsArray);
    client->write(jsonDoc.toJson());
    qDebug() << "Sent operations to client " + client->peerAddress().toString() + " data: " << jsonDoc.toJson();
}
void Server::sendSystemInfoToClient(QTcpSocket* client) {
    QJsonObject systemInfoResponse;
    systemInfoResponse["message"] = "System info received by server (Qt)";
    QJsonDocument jsonDoc(systemInfoResponse);
    client->write(jsonDoc.toJson());
    qDebug() << "Sent response to " +  client->peerAddress().toString() + " " << jsonDoc.toJson();
}
void Server::onDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if(client) {
        clients.remove(client);
        qDebug() << "Client disconnected from " << client->peerAddress().toString();
        client->deleteLater();
    }
}
void Server::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response: " << response;
        if(reply->url().toString() == "http://localhost:1234/taskStateChanged"){
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &jsonError);
            if(jsonError.error == QJsonParseError::NoError){
                if(jsonDoc.isObject()) {
                    QJsonObject taskStateChange = jsonDoc.object();
                    if(taskStateChange.contains("id") && taskStateChange.contains("enabled")){
                        handleTaskStateChanged(taskStateChange, nullptr);
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
Server::~Server(){
    saveTasksToFile();
    for (const auto& task : tasks) {
        for (const auto& trigger : task->getTriggers()) {
            delete trigger;
        }
        for(const auto &operation : task->getOperations()){
            delete operation;
        }
        delete task;
    }
}
