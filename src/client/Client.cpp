#include "Client.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostInfo>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
Client::Client(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this))
{
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::onDisconnected);

    logFile.setFileName(QDir::currentPath() + "/client.log");
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open log file " + logFile.fileName() + " " + logFile.errorString();
    }
    loadConfig();
    logMessage("Client started.");
}

Client::~Client()
{
    if (logFile.isOpen()) {
        logFile.close();
    }
}
bool Client::shouldLog(const QString& level) const {
    if (logLevel == "none") return false;
    if (logLevel == "debug") return true;
    if (level == "info" && (logLevel == "info" || logLevel == "warning" || logLevel == "error")) return true;
    if (level == "warning" && (logLevel == "warning" || logLevel == "error")) return true;
    if (level == "error" && logLevel == "error") return true;
    return false;
}
void Client::logMessage(const QString& message) {
    if (shouldLog("debug")) {
        qDebug() << message;
    }
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") << message << "\n";
        logFile.flush();
    }
}
void Client::loadConfig()
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
void Client::onConnected() {
    if (shouldLog("info"))
        logMessage("Connected to server.");
    sendSystemInfo();
}
void Client::onReadyRead() {
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
                }else{
                    if (shouldLog("warning"))
                        logMessage("Authorization failed!");
                    tcpSocket->disconnectFromHost();
                }
            }
        }

    }
}
void Client::onDisconnected() {
    if (shouldLog("info"))
        logMessage("Disconnected from server.");
}
void Client::sendSystemInfo() {
    QJsonObject systemInfo;
    systemInfo["hostname"] = QHostInfo::localHostName();
    systemInfo["username"] = QString::fromUtf8(qgetenv("USERNAME"));

    QJsonDocument jsonDoc(systemInfo);
    tcpSocket->write(jsonDoc.toJson());
    if (shouldLog("debug"))
        logMessage("Sent system info to server: " + QString(jsonDoc.toJson()));
}
