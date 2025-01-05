#include "Agent.h"
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHostInfo>
#include <QDir>
#include <QUrlQuery>
#include <QCoreApplication>
#include "PingTrigger.h"
#include "DiskSpaceTrigger.h"
#include "TriggerWatchDog.h"
#include <QJsonArray>

Agent::Agent(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)), manager(new QNetworkAccessManager(this)) {
    connect(socket, &QTcpSocket::connected, this, &Agent::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &Agent::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Agent::onDisconnected);
    connect(manager, &QNetworkAccessManager::finished, this, &Agent::onReply);
    connectToServer();
}

void Agent::sendSystemInfo() {
    QString hostname = QHostInfo::localHostName();
    QString username = QDir::homePath().split("/").last();
    QJsonObject systemInfo;
    systemInfo["hostname"] = hostname;
    systemInfo["username"] = username;
    QJsonDocument jsonDoc(systemInfo);
    QByteArray jsonData = jsonDoc.toJson();
    QUrl url("http://localhost:1234/system");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager->post(request, jsonData);
    qDebug() << "Sent System info:\n " << jsonData;
}

void Agent::connectToServer() {
    socket->connectToHost("localhost", 1234);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "Error: " << socket->errorString();
        QCoreApplication::exit(1);
    }
    sendSystemInfo();
}

void Agent::sendMessage(const QString &message) {
    QUrl url("http://localhost:1234/message");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonMessage;
    jsonMessage["message"] = message;
    QJsonDocument jsonDoc(jsonMessage);
    QByteArray jsonData = jsonDoc.toJson();

    manager->post(request, jsonData);
    qDebug() << "Sent message: " << jsonData;
}

void Agent::loadTriggers() {
    QUrl url("http://localhost:1234/triggers");
    QNetworkRequest request(url);
    manager->get(request);
    qDebug() << "Sent get triggers request";
}

void Agent::loadOperations() {
    QUrl url("http://localhost:1234/operations");
    QNetworkRequest request(url);
    manager->get(request);
    qDebug() << "Sent get operations request";
}

void Agent::onConnected() {
    qDebug() << "Connected to server.";
    loadTriggers();
    loadOperations();
}

void Agent::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received data from server: " << data;
}

void Agent::onDisconnected() {
    qDebug() << "Disconnected from server.";
    QCoreApplication::exit(0);
}

void Agent::sendOperationResult(const QJsonObject &operationResult) {
    QUrl url("http://localhost:1234/operationResult");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument jsonDoc(operationResult);
    QByteArray jsonData = jsonDoc.toJson();
    manager->post(request, jsonData);
    qDebug() << "Sent operation result: " << jsonData;
}

void Agent::performOperation(const QJsonObject &operation) {
    QString operationName = operation["name"].toString();
    QJsonObject parameters = operation["parameters"].toObject();
    qDebug() << "Performing operation: " << operationName << " with params " << parameters;
    QJsonObject result;
    result["id"] = operation["id"];
    if (operationName == "testOperation") {
        result["result"] = "Test operation performed by agent";
    } else {
        result["result"] = "Operation " + operationName + " is not supported by agent";
    }
    sendOperationResult(result);
}

void Agent::onReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        qDebug() << "Response from server: " << response;
        if (response.contains("System info received by server (Qt)")) {
            sendMessage("Hello from agent");
        } else {
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &jsonError);
            if (jsonError.error != QJsonParseError::NoError) {
                qDebug() << "Error: Could not parse system info: " << jsonError.errorString();
                return;
            }
            if (jsonDoc.isObject()) {
                QJsonObject triggers = jsonDoc.object();
                if (triggers.contains("ping") && triggers["ping"].isArray()) {
                    QJsonArray pingArray = triggers["ping"].toArray();
                    for (const auto &value : pingArray) {
                        if (value.isObject()) {
                            QJsonObject pingConfig = value.toObject();
                            if (pingConfig.contains("target") && pingConfig.contains("interval")) {
                                QString target = pingConfig["target"].toString();
                                int interval = pingConfig["interval"].toInt();
                                PingTrigger *trigger = new PingTrigger(target, interval, this);
                                connect(trigger, &Trigger::triggered, this, &Agent::onTriggered);
                                trigger->start();
                                this->triggers.append(trigger);
                            }
                        }
                    }
                }
                if (triggers.contains("diskSpace") && triggers["diskSpace"].isArray()) {
                    QJsonArray diskSpaceArray = triggers["diskSpace"].toArray();
                    for (const auto &value : diskSpaceArray) {
                        if (value.isObject()) {
                            QJsonObject diskSpaceConfig = value.toObject();
                            if (diskSpaceConfig.contains("disks") && diskSpaceConfig.contains("threshold") &&
                                diskSpaceConfig.contains("interval")) {
                                QStringList disks;
                                if (diskSpaceConfig["disks"].isArray()) {
                                    QJsonArray disksArray = diskSpaceConfig["disks"].toArray();
                                    for (const auto &diskValue : disksArray) {
                                        if (diskValue.isString()) {
                                            disks.append(diskValue.toString());
                                        }
                                    }
                                }
                                int threshold = diskSpaceConfig["threshold"].toInt();
                                int interval = diskSpaceConfig["interval"].toInt();
                                DiskSpaceTrigger *trigger = new DiskSpaceTrigger(disks, threshold, interval, this);
                                connect(trigger, &Trigger::triggered, this, &Agent::onTriggered);
                                trigger->start();
                                this->triggers.append(trigger);
                            }
                        }
                    }
                }
                if (triggers.contains("watchDog") && triggers["watchDog"].isArray()) {
                    QJsonArray watchDogArray = triggers["watchDog"].toArray();
                    for (const auto &value : watchDogArray) {
                        if (value.isObject()) {
                            QJsonObject watchDogConfig = value.toObject();
                            if (watchDogConfig.contains("path") && watchDogConfig.contains("mask") &&
                                watchDogConfig.contains("interval")) {
                                QString path = watchDogConfig["path"].toString();
                                QString mask = watchDogConfig["mask"].toString();
                                int interval = watchDogConfig["interval"].toInt();
                                TriggerWatchDog *trigger = new TriggerWatchDog(path, mask, interval, this);
                                connect(trigger, &Trigger::triggered, this, &Agent::onTriggered);
                                trigger->start();
                                this->triggers.append(trigger);
                            }
                        }
                    }
                }
            }
            else if (jsonDoc.isArray()) {
                QJsonArray operationsArray = jsonDoc.array();
                for (const auto &value : operationsArray) {
                    if (value.isObject()) {
                        performOperation(value.toObject());
                    }
                }
            }
        }
    } else {
        qDebug() << "Error from server: " << reply->errorString();
        socket->disconnectFromHost();
    }
    reply->deleteLater();
}

void Agent::onTriggered(const QJsonObject &result) {
    qDebug() << "Triggered with value: " << result;
}

Agent::~Agent() {
    for (const auto &trigger : triggers) {
        trigger->stop();
        delete trigger;
    }
}
