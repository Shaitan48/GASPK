#include "Server.h"
#include <QTcpSocket>
#include <QDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QNetworkReply>
#include <QFile>
#include <QJsonParseError>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

Server::Server(QObject *parent) : QObject(parent), manager(new QNetworkAccessManager(this)), server(new QTcpServer(this)) {
    loadConfig();
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    connect(manager, &QNetworkAccessManager::finished, this, &Server::onReply);
    if (server->listen(QHostAddress::Any, serverPort)) {
        qDebug() << getFormattedDateTime() << "Server started on port " << serverPort;
    } else {
        qDebug() << getFormattedDateTime() << "Server failed to start";
        QCoreApplication::exit(1);
    }
}
QString Server::getFormattedDateTime() {
    QDateTime now = QDateTime::currentDateTime();
    return "[" + now.toString("dd.MM.yyyy hh:mm:ss") + "] ";
}

void Server::loadConfig() {
    QString configFilePath = QCoreApplication::applicationDirPath() + "/config.json";
    QFileInfo checkFile(configFilePath);
    if(!checkFile.exists() || !checkFile.isFile()){
        qDebug() << getFormattedDateTime() <<  "Error: config.json does not exist " << configFilePath;
        serverPort = 1234;
        return;
    }

    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << getFormattedDateTime() << "Error: Could not open config.json";
        serverPort = 1234;
        return;
    }

    QByteArray configData = configFile.readAll();
    configFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(configData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << getFormattedDateTime() << "Error: Could not parse config.json: " << jsonError.errorString();
        serverPort = 1234;
        return;
    }

    if (!jsonDoc.isObject()) {
        qDebug() << getFormattedDateTime() << "Error: config.json is not a JSON object";
        serverPort = 1234;
        return;
    }
    QJsonObject config = jsonDoc.object();

    if(config.contains("server") && config["server"].isObject()){
        QJsonObject serverConfig = config["server"].toObject();
        if (serverConfig.contains("port") && serverConfig["port"].isDouble()){
            serverPort = serverConfig["port"].toInt();
        }
    }else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have server object";
        serverPort = 1234;
    }
}
void Server::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << getFormattedDateTime() << "Response from network: " << response;
    }
    else
    {
        qDebug() << getFormattedDateTime() << "Error from network: " << reply->errorString();
    }
    reply->deleteLater();
}

void Server::onNewConnection() {
    QTcpSocket *socket = server->nextPendingConnection();
    if (!socket)
    {
        qDebug() << getFormattedDateTime() << "Error: Null socket";
        return;
    }
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        handleClient(socket);
    });
}

void Server::handleClient(QTcpSocket *socket) {
    if (socket->bytesAvailable() == 0)
        return;

    QByteArray data = socket->readAll();
    QString request(data);
    qDebug() << getFormattedDateTime() << "Received request:\n" << request;
    QStringList lines = request.split("\r\n");
    if(lines.isEmpty())
        return;
    QString firstLine = lines.first();
    qDebug() << getFormattedDateTime() << "First line of request:\n" << firstLine;
    QStringList parts = firstLine.split(" ");
    if (parts.size() < 3) {
        qDebug() << getFormattedDateTime() << "Invalid HTTP request format";
        return;
    }

    QString method = parts[0];
    QString path = parts[1];

    QByteArray responseData;
    QString contentType;

    if (method == "GET" && path == "/message")
    {
        //можно отправить запрос самому себе
        // QUrl url("http://localhost:1234/message");
        // QNetworkRequest request(url);
        //manager->get(request);

        QJsonObject json;
        json["message"] = "Hello from server (Qt)";
        QJsonDocument jsonDoc(json);
        responseData = jsonDoc.toJson();
        contentType = "application/json";

    }
    else if (method == "POST" && path == "/system")
    {
        QString body;
        bool foundBody = false;
        for (int i = 0; i < lines.size(); ++i)
        {
            if(foundBody){
                body.append(lines.at(i));
            }
            if (lines.at(i).startsWith("\r\n")){
                foundBody = true;
            }
        }
        qDebug() <<  getFormattedDateTime() << "Received system info:\n" << body;
        QJsonObject json;
        json["message"] = "System info received by server (Qt)";
        QJsonDocument jsonDoc(json);
        responseData = jsonDoc.toJson();
        contentType = "application/json";
    }
    else if (method == "POST" && path == "/message")
    {
        // Обработка POST запроса.
        QString body;
        bool foundBody = false;
        for (int i = 0; i < lines.size(); ++i)
        {
            if(foundBody){
                body.append(lines.at(i));
            }
            if (lines.at(i).startsWith("\r\n")){
                foundBody = true;
            }

        }
        qDebug() << getFormattedDateTime() << "Post request body:\n" << body;
        QJsonObject json;
        json["message"] = "Message received by server (Qt)";
        QJsonDocument jsonDoc(json);
        responseData = jsonDoc.toJson();
        contentType = "application/json";
    }else {
        qDebug() << getFormattedDateTime() << "Error: Invalid request";
        socket->write("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found");
        socket->disconnectFromHost();
        return;
    }

    // Формируем HTTP ответ.
    QByteArray httpResponse = "HTTP/1.1 200 OK\r\n";
    httpResponse += "Content-Type: " + contentType.toUtf8() + "\r\n";
    httpResponse += "Content-Length: " + QByteArray::number(responseData.size()) + "\r\n";
    httpResponse += "\r\n";
    httpResponse += responseData;

    socket->write(httpResponse);
    socket->disconnectFromHost();
}
