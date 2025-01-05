#include "Client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include "Task.h"
#include "Operation.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QMap>
Client::Client(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this))
{
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    loadConfig();
    qDebug() << "Client started.";
}
Client::~Client() {
    for(const auto& task : tasks){
        delete task;
    }
}
void Client::loadConfig()
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
void Client::onConnected() {
    qDebug() << "Connected to server.";
}
void Client::onReadyRead() {
    QByteArray data = tcpSocket->readAll();
    qDebug() << "Data from server: " << data;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if(jsonDoc.isArray()){
            QJsonArray operationsArray = jsonDoc.array();
            for(const auto &operationValue : operationsArray){
                if(operationValue.isObject()){
                    QJsonObject operationJson = operationValue.toObject();
                    if(operationJson.contains("id") && operationJson.contains("enabled")){
                        qlonglong id = operationJson["id"].toVariant().toLongLong();
                        bool enabled = operationJson["enabled"].toBool();
                        Task* task = new Task(id, this);
                        connect(task, &Task::stateChanged, this, &Client::handleTaskStateChanged);
                        task->setEnabled(enabled);
                        tasks.append(task);
                        if(operationJson.contains("name")){
                            QString operationName = operationJson["name"].toString();
                            if(operationName == "fileDelete") {
                                QJsonObject parameters;
                                QJsonObject taskStateChange;
                                taskStateChange["id"] = (qlonglong)id;
                                taskStateChange["enabled"] = false;
                                task->stateChanged(taskStateChange, nullptr);
                            }
                            else if (operationName == "fileCopy"){
                                QJsonObject parameters;
                                QJsonObject taskStateChange;
                                taskStateChange["id"] = (qlonglong)id;
                                taskStateChange["enabled"] = false;
                                task->stateChanged(taskStateChange, nullptr);
                            } else if (operationName == "fileMove"){
                                QJsonObject parameters;
                                QJsonObject taskStateChange;
                                taskStateChange["id"] = (qlonglong)id;
                                taskStateChange["enabled"] = false;
                                task->stateChanged(taskStateChange, nullptr);
                            }
                            else if (operationName == "message"){
                                QJsonObject parameters;
                                QJsonObject taskStateChange;
                                taskStateChange["id"] = (qlonglong)id;
                                taskStateChange["enabled"] = false;
                                task->stateChanged(taskStateChange, nullptr);
                            }
                            else if (operationName == "request"){
                                QJsonObject parameters;
                                QJsonObject taskStateChange;
                                taskStateChange["id"] = (qlonglong)id;
                                taskStateChange["enabled"] = false;
                                task->stateChanged(taskStateChange, nullptr);
                            }
                        }
                    }
                }

            }
        }
    }
}
void Client::onDisconnected() {
    qDebug() << "Disconnected from server.";
}
void Client::handleTaskStateChanged(const QJsonObject& taskStateChange, QTcpSocket* client) {
    if(taskStateChange.contains("id") && taskStateChange.contains("enabled")){
        qlonglong id = taskStateChange["id"].toVariant().toLongLong();
        bool enabled = taskStateChange["enabled"].toBool();
        qDebug() << "Task " + QString::number(id) + " state changed. Enabled: " << enabled;
    }
}
