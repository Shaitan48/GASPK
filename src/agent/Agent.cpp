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
#include "../common/PingTrigger.h"
#include "../common/DiskSpaceTrigger.h"
#include "../common/TriggerWatchDog.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QTextStream>
#include <QTimer>
Agent::Agent(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this)), manager(new QNetworkAccessManager(this))
{
    connect(tcpSocket, &QTcpSocket::connected, this, &Agent::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Agent::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Agent::onDisconnected);
    connect(manager, &QNetworkAccessManager::finished, this, &Agent::onReply);

    logFile.setFileName(QDir::currentPath() + "/agent.log");
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open log file " + logFile.fileName() + " " + logFile.errorString();
    }
    loadConfig();
    logMessage("Agent started.");
}
bool Agent::shouldLog(const QString& level) const {
    if (logLevel == "none") return false;
    if (logLevel == "debug") return true;
    if (level == "info" && (logLevel == "info" || logLevel == "warning" || logLevel == "error")) return true;
    if (level == "warning" && (logLevel == "warning" || logLevel == "error")) return true;
    if (level == "error" && logLevel == "error") return true;
    return false;
}
void Agent::logMessage(const QString& message) {
    if (shouldLog("debug")) {
        qDebug() << message;
    }

    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") << message << "\n";
        logFile.flush();
    }
}
void Agent::loadConfig()
{
    QFile file(QDir::currentPath() + "/config.json");
    if (!file.open(QIODevice::ReadOnly)) {
        logMessage("Could not open config file " + file.fileName() + " " + file.errorString());
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        logMessage("Could not parse json from config file " + file.fileName() + " " + jsonError.errorString());
        return;
    }
    if(jsonDoc.isObject()){
        QJsonObject config = jsonDoc.object();
        if (config.contains("server") && config["server"].isObject()) {
            QJsonObject serverConfig = config["server"].toObject();
            if(serverConfig.contains("host") && serverConfig.contains("port")){
                host = serverConfig["host"].toString();
                port = serverConfig["port"].toInt();
                tcpSocket->connectToHost(host, port);
            }

        }
        if(config.contains("logLevel")){
            logLevel = config["logLevel"].toString("debug");
        }
    }
}
void Agent::onConnected() {
    if (shouldLog("info"))
        logMessage("Connected to server.");
    sendSystemInfo();
}
void Agent::onReadyRead() {
    QByteArray data = tcpSocket->readAll();
    if (shouldLog("debug"))
        logMessage("Data from server: " + QString(data));
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error == QJsonParseError::NoError){
        if(jsonDoc.isObject()){
            QJsonObject json = jsonDoc.object();
            if(json.contains("authResult"))
            {
                bool authResult = json["authResult"].toBool();
                if (authResult)
                {
                    if (shouldLog("info"))
                        logMessage("Authorization success.");
                    if (json.contains("tasks") && json["tasks"].isArray()){
                        QJsonArray tasksArray = json["tasks"].toArray();
                        for (const auto& taskValue : tasksArray){
                            if(taskValue.isObject()){
                                QJsonObject taskJson = taskValue.toObject();
                                if(taskJson.contains("id")){
                                    qlonglong id = taskJson["id"].toVariant().toLongLong();
                                    bool enabled = false;
                                    if(taskJson.contains("enabled") && taskJson["enabled"].isBool()){
                                        enabled = taskJson["enabled"].toBool();
                                        if (shouldLog("info"))
                                            logMessage("Task " + QString::number(id) + " state changed. Enabled: " + QString(enabled ? "true" : "false"));
                                    }
                                    if(taskJson.contains("ping") && taskJson["ping"].isArray()){
                                        QJsonArray pingArray = taskJson["ping"].toArray();
                                        for(const auto &pingValue : pingArray){
                                            if(pingValue.isObject()){
                                                QJsonObject pingConfig = pingValue.toObject();
                                                if(pingConfig.contains("target") && pingConfig.contains("interval")) {
                                                    QString target = pingConfig["target"].toString();
                                                    int interval = pingConfig["interval"].toInt();
                                                    PingTrigger *trigger = new PingTrigger(id, nullptr, target, interval, this);
                                                    if (enabled )
                                                        trigger->start();
                                                    triggers.append(trigger);
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
                                                    DiskSpaceTrigger *trigger = new DiskSpaceTrigger(id, nullptr,disks, threshold, interval, this);
                                                    if (enabled )
                                                        trigger->start();
                                                    triggers.append(trigger);
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
                                                    TriggerWatchDog *trigger = new TriggerWatchDog(id, nullptr,path, mask, interval, this);
                                                    if (enabled )
                                                        trigger->start();
                                                    triggers.append(trigger);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }else{
                    if (shouldLog("warning"))
                        logMessage("Authorization failed!");
                    tcpSocket->disconnectFromHost();
                }
            }

        }
    }
}
void Agent::onDisconnected() {
    if (shouldLog("info"))
        logMessage("Disconnected from server.");
}
void Agent::sendSystemInfo() {
    QJsonObject systemInfo;
    systemInfo["hostname"] = QHostInfo::localHostName();
    systemInfo["username"] = QString::fromUtf8(qgetenv("USERNAME"));

    QJsonDocument jsonDoc(systemInfo);
    tcpSocket->write(jsonDoc.toJson());
    if (shouldLog("debug"))
        logMessage("Sent system info to server: " + QString(jsonDoc.toJson()));
}
void Agent::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        if (shouldLog("debug"))
            logMessage("Response: " + QString(response));
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
                        for(Trigger* trigger : triggers){
                            if(trigger->id() == id){
                                if(enabled){
                                    trigger->start();
                                }else {
                                    trigger->stop();
                                }
                                if (shouldLog("info"))
                                    logMessage("Task " + QString::number(id) + " state changed. Enabled: " + QString(enabled ? "true" : "false"));
                            }

                        }

                    }
                }
            }
        }
    }
    else
    {
        if (shouldLog("error"))
            logMessage("Error: " + reply->errorString());
    }
    reply->deleteLater();
}

Agent::~Agent()
{
    if (logFile.isOpen()) {
        logFile.close();
    }
    for(const auto& trigger: triggers){
        delete trigger;
    }
}
