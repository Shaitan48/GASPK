#include "Server.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "../common/PingTrigger.h"
#include "../common/DiskSpaceTrigger.h"
#include "../common/TriggerWatchDog.h"
Server::Server(QObject *parent)
    : QObject{parent}, tcpServer(new QTcpServer(this)), manager(new QNetworkAccessManager(this)),db(new SqlDatabase(this))
{
    if (tcpServer->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server listening on port 1234";
    } else {
        qDebug() << "Server could not start listening";
    }
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    connect(manager, &QNetworkAccessManager::finished, this, &Server::onReply);
    loadTasksFromFile();
}
Server::~Server()
{
    delete db;
    for(auto task : tasks){
        delete task;
    }
}

void Server::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    if (clientSocket) {
        connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
        clients[clientSocket] = "unknown";
        qDebug() << "New connection from " << clientSocket->peerAddress().toString();
    }

}
void Server::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) {
        return;
    }
    QByteArray data = clientSocket->readAll();
    qDebug() << "Data from " << clientSocket->peerAddress().toString() << ": " << data;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if (jsonDoc.isObject()){
            QJsonObject json = jsonDoc.object();
            if (json.contains("hostname") && json.contains("username")){
                QString hostname = json["hostname"].toString();
                QString username = json["username"].toString();
                if (isAgentAllowed(username, hostname)){
                    clients[clientSocket] = "agent";
                    sendAuthResult(clientSocket, true);
                    sendTriggersToAgent(clientSocket);
                }
                else {
                    sendAuthResult(clientSocket, false);
                    clientSocket->disconnectFromHost();
                }

            } else {
                clients[clientSocket] = "client";
                sendAuthResult(clientSocket, true);
                sendOperationsToClient(clientSocket);
                sendSystemInfoToClient(clientSocket);
            }
        }
    }

}
void Server::onDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        qDebug() << "Client disconnected: " << clientSocket->peerAddress().toString();
        clients.remove(clientSocket);
        clientSocket->deleteLater();
    }
}
void Server::loadTasksFromFile()
{
    QFile file(QDir::currentPath() + "/tasks.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            if (jsonDoc.isArray()) {
                QJsonArray tasksArray = jsonDoc.array();
                for (const auto& taskValue : tasksArray){
                    if(taskValue.isObject()){
                        QJsonObject taskJson = taskValue.toObject();
                        if(taskJson.contains("id")) {
                            qlonglong id = taskJson["id"].toVariant().toLongLong();
                            Task* task = new Task(id,this);
                            if(taskJson.contains("enabled")){
                                task->setEnabled(taskJson["enabled"].toBool());
                            }
                            if(taskJson.contains("ping") && taskJson["ping"].isArray()){
                                QJsonArray pingArray = taskJson["ping"].toArray();
                                for(const auto &pingValue : pingArray){
                                    if(pingValue.isObject()){
                                        QJsonObject pingConfig = pingValue.toObject();
                                        if(pingConfig.contains("target") && pingConfig.contains("interval")) {
                                            QString target = pingConfig["target"].toString();
                                            int interval = pingConfig["interval"].toInt();
                                            //PingTrigger *trigger = new PingTrigger(id,task, target, interval, this);
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
                                            DiskSpaceTrigger *trigger = new DiskSpaceTrigger(id,task,disks, threshold, interval, this);
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
                                            TriggerWatchDog *trigger = new TriggerWatchDog(id,task,path, mask, interval, this);
                                            task->addTrigger(trigger);
                                        }
                                    }
                                }
                            }
                            tasks.append(task);
                        }
                    }
                }
                qDebug() << "Tasks loaded from file.";
            }
        }
    }
}
void Server::saveTasksToFile() const
{
    QFile file(QDir::currentPath() + "/tasks.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QJsonArray tasksArray;
        for (const auto& task : tasks) {
            QJsonObject taskJson = task->toJson();
            QJsonArray pingArray;
            for (const auto& trigger : task->getTriggers()) {
                if(PingTrigger* pingTrigger = dynamic_cast<PingTrigger*>(trigger)){
                    QJsonObject pingJson;
                    pingJson["target"] = pingTrigger->target();
                    pingJson["interval"] = pingTrigger->interval();
                    pingArray.append(pingJson);
                }
            }
            if(!pingArray.isEmpty()){
                taskJson["ping"] = pingArray;
            }
            QJsonArray diskSpaceArray;
            for (const auto& trigger : task->getTriggers()) {
                if(DiskSpaceTrigger* diskSpaceTrigger = dynamic_cast<DiskSpaceTrigger*>(trigger)){
                    QJsonObject diskSpaceJson;
                    diskSpaceJson["disks"] = QJsonArray::fromStringList(diskSpaceTrigger->disks());
                    diskSpaceJson["threshold"] = diskSpaceTrigger->threshold();
                    diskSpaceJson["interval"] = diskSpaceTrigger->interval();
                    diskSpaceArray.append(diskSpaceJson);
                }
            }
            if(!diskSpaceArray.isEmpty()){
                taskJson["diskSpace"] = diskSpaceArray;
            }
            QJsonArray watchDogArray;
            for (const auto& trigger : task->getTriggers()) {
                if(TriggerWatchDog* watchDogTrigger = dynamic_cast<TriggerWatchDog*>(trigger)){
                    QJsonObject watchDogJson;
                    watchDogJson["path"] = watchDogTrigger->path();
                    watchDogJson["mask"] = watchDogTrigger->mask();
                    watchDogJson["interval"] = watchDogTrigger->interval();
                    watchDogArray.append(watchDogJson);
                }
            }
            if(!watchDogArray.isEmpty()){
                taskJson["watchDog"] = watchDogArray;
            }

            tasksArray.append(taskJson);
        }
        QJsonDocument jsonDoc(tasksArray);
        QTextStream out(&file);
        out << jsonDoc.toJson();
        file.close();
        qDebug() << "Tasks saved to file.";
    }
    else
    {
        qDebug() << "Could not save tasks to file.";
    }
}
void Server::sendTriggersToAgent(QTcpSocket* agent)
{
    QJsonArray tasksArray;
    for (const auto& task : tasks) {
        QJsonObject taskJson = task->toJson();
        QJsonArray pingArray;
        for (const auto& trigger : task->getTriggers()) {
            if(PingTrigger* pingTrigger = dynamic_cast<PingTrigger*>(trigger)){
                QJsonObject pingJson;
                pingJson["target"] = pingTrigger->target();
                pingJson["interval"] = pingTrigger->interval();
                pingArray.append(pingJson);
            }
        }
        if(!pingArray.isEmpty()){
            taskJson["ping"] = pingArray;
        }
        QJsonArray diskSpaceArray;
        for (const auto& trigger : task->getTriggers()) {
            if(DiskSpaceTrigger* diskSpaceTrigger = dynamic_cast<DiskSpaceTrigger*>(trigger)){
                QJsonObject diskSpaceJson;
                diskSpaceJson["disks"] = QJsonArray::fromStringList(diskSpaceTrigger->disks());
                diskSpaceJson["threshold"] = diskSpaceTrigger->threshold();
                diskSpaceJson["interval"] = diskSpaceTrigger->interval();
                diskSpaceArray.append(diskSpaceJson);
            }
        }
        if(!diskSpaceArray.isEmpty()){
            taskJson["diskSpace"] = diskSpaceArray;
        }
        QJsonArray watchDogArray;
        for (const auto& trigger : task->getTriggers()) {
            if(TriggerWatchDog* watchDogTrigger = dynamic_cast<TriggerWatchDog*>(trigger)){
                QJsonObject watchDogJson;
                watchDogJson["path"] = watchDogTrigger->path();
                watchDogJson["mask"] = watchDogTrigger->mask();
                watchDogJson["interval"] = watchDogTrigger->interval();
                watchDogArray.append(watchDogJson);
            }
        }
        if(!watchDogArray.isEmpty()){
            taskJson["watchDog"] = watchDogArray;
        }
        tasksArray.append(taskJson);

    }
    QJsonObject response;
    response["tasks"] = tasksArray;
    QJsonDocument jsonDoc(response);
    agent->write(jsonDoc.toJson());
    qDebug() << "Send tasks to " << agent->peerAddress().toString();
}
void Server::sendOperationsToClient(QTcpSocket* client)
{

}
void Server::sendAuthResult(QTcpSocket* client, bool authResult)
{
    QJsonObject response;
    response["authResult"] = authResult;
    QJsonDocument jsonDoc(response);
    client->write(jsonDoc.toJson());
    qDebug() << "Sent auth result to " << client->peerAddress().toString();
}
void Server::sendSystemInfoToClient(QTcpSocket* client)
{
    QJsonObject systemInfo;
    systemInfo["hostname"] = QHostInfo::localHostName();
    systemInfo["username"] = QString::fromUtf8(qgetenv("USERNAME"));

    QJsonDocument jsonDoc(systemInfo);
    client->write(jsonDoc.toJson());
    qDebug() << "Sent system info to client " << client->peerAddress().toString();
}
void Server::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response: " << response;
    }
    else
    {
        qDebug() << "Error: " << reply->errorString();
    }
    reply->deleteLater();
}
bool Server::isAgentAllowed(const QString &username, const QString &hostname) const
{
    QFile file(QDir::currentPath() + "/agents.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        if (jsonError.error == QJsonParseError::NoError){
            if (jsonDoc.isArray()) {
                QJsonArray agentsArray = jsonDoc.array();
                for (const auto& agentValue : agentsArray){
                    if (agentValue.isObject()){
                        QJsonObject agentJson = agentValue.toObject();
                        if(agentJson.contains("username") && agentJson.contains("hostname")){
                            if(agentJson["username"].toString() == username && agentJson["hostname"].toString() == hostname){
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
void Server::handleTaskStateChanged(const QJsonObject& taskStateChange, QTcpSocket* client)
{
    QNetworkRequest request(QUrl("http://localhost:1234/taskStateChanged"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument doc(taskStateChange);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = manager->post(request, data);

}
