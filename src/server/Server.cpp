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
#include <QJsonArray>
#include <QSqlError>
#include <QSqlQuery>
#include "PingTrigger.h"
#include "DiskSpaceTrigger.h"
#include "TriggerWatchDog.h"

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

    db = QSqlDatabase::addDatabase("QODBC");
    QString connectionString = QString("Driver={ODBC Driver 17 for SQL Server};Server=%1,%2;Database=%3;").arg(dbHost).arg(dbPort).arg(dbName);
    db.setDatabaseName(connectionString);
    db.setUserName(dbUsername);
    db.setPassword(dbPassword);
    if(!db.open()){
        qDebug() << getFormattedDateTime() <<  "Error: Could not connect to database: " << db.lastError().text();
        QCoreApplication::exit(1);
    }else{
        qDebug() << getFormattedDateTime() << "Connected to database" ;
        QSqlQuery query("SELECT @@VERSION;");
        if (query.next()) {
            qDebug() << getFormattedDateTime() << "Database version:" << query.value(0).toString();
        }
    }

}
Server::~Server(){
    db.close();
    qDebug() << getFormattedDateTime() << "Database closed" ;

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
    config = jsonDoc.object();

    if(config.contains("server") && config["server"].isObject()){
        QJsonObject serverConfig = config["server"].toObject();
        if (serverConfig.contains("port") && serverConfig["port"].isDouble()){
            serverPort = serverConfig["port"].toInt();
        }
    }else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have server object";
        serverPort = 1234;
    }

    if(config.contains("allowedAgents") && config["allowedAgents"].isArray()){
        QJsonArray allowedAgentsArray = config["allowedAgents"].toArray();
        for (const auto& value : allowedAgentsArray) {
            if(value.isObject()){
                QJsonObject agent = value.toObject();
                if(agent.contains("username") && agent.contains("hostname")){
                    AuthAgent authAgent;
                    authAgent.username = agent["username"].toString();
                    authAgent.hostname =  agent["hostname"].toString();
                    allowedAgents.append(authAgent);
                }

            }
        }
    }else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have allowedAgents array";
    }
    if(config.contains("database") && config["database"].isObject()){
        QJsonObject dbConfig = config["database"].toObject();
        if (dbConfig.contains("host") && dbConfig["host"].isString()){
            dbHost = dbConfig["host"].toString();
        }
        if (dbConfig.contains("port") && dbConfig["port"].isDouble()){
            dbPort = dbConfig["port"].toInt();
        }
        if (dbConfig.contains("username") && dbConfig["username"].isString()){
            dbUsername = dbConfig["username"].toString();
        }
        if (dbConfig.contains("password") && dbConfig["password"].isString()){
            dbPassword = dbConfig["password"].toString();
        }
        if (dbConfig.contains("database") && dbConfig["database"].isString()){
            dbName = dbConfig["database"].toString();
        }
    }else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have database object";
    }
    if(config.contains("triggers") && config["triggers"].isObject()){
        QJsonObject triggersConfig = config["triggers"].toObject();
        if(triggersConfig.contains("ping") && triggersConfig["ping"].isArray()){
            QJsonArray pingArray = triggersConfig["ping"].toArray();
            qDebug() << getFormattedDateTime() << "Loaded " << pingArray.size() << " ping triggers from config";
        }

        if(triggersConfig.contains("diskSpace") && triggersConfig["diskSpace"].isArray()){
            QJsonArray diskSpaceArray = triggersConfig["diskSpace"].toArray();
            qDebug() << getFormattedDateTime() << "Loaded " << diskSpaceArray.size() << " diskSpace triggers from config";
        }
        if(triggersConfig.contains("watchDog") && triggersConfig["watchDog"].isArray()){
            QJsonArray watchDogArray = triggersConfig["watchDog"].toArray();
            qDebug() << getFormattedDateTime() << "Loaded " << watchDogArray.size() << " watchDog triggers from config";
        }

    }else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have triggers object";
    }
    if(config.contains("operations") && config["operations"].isArray()){
        QJsonArray operationsArray = config["operations"].toArray();
        qDebug() << getFormattedDateTime() << "Loaded " << operationsArray.size() << " operations from config";
    }
    else{
        qDebug() << getFormattedDateTime() << "Error: config.json does not have operations object";
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
    qDebug() << getFormattedDateTime() << "Received request:\n" << data;

    int headerEnd = data.indexOf("\r\n\r\n");

    if (headerEnd == -1) {
        qDebug() << getFormattedDateTime() << "Error: Invalid HTTP request format";
        return;
    }

    QByteArray headerData = data.left(headerEnd);
    QByteArray bodyData = data.mid(headerEnd + 4);
    QString header(headerData);
    QStringList lines = header.split("\r\n");
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
        QJsonObject json;
        json["message"] = "Hello from server (Qt)";
        QJsonDocument jsonDoc(json);
        responseData = jsonDoc.toJson();
        contentType = "application/json";

    }
    else if (method == "GET" && path == "/triggers")
    {
        QJsonObject triggers;
        QJsonArray pingTriggers;
        QJsonArray diskSpaceTriggers;
        QJsonArray watchDogTriggers;

        if(config.contains("triggers") && config["triggers"].isObject()){
            QJsonObject triggersConfig = config["triggers"].toObject();
            if(triggersConfig.contains("ping") && triggersConfig["ping"].isArray()){
                QJsonArray pingArray = triggersConfig["ping"].toArray();
                for(const auto& value: pingArray){
                    pingTriggers.append(value);
                }
            }
            if(triggersConfig.contains("diskSpace") && triggersConfig["diskSpace"].isArray()){
                QJsonArray diskSpaceArray = triggersConfig["diskSpace"].toArray();
                for(const auto& value: diskSpaceArray){
                    diskSpaceTriggers.append(value);
                }
            }
            if(triggersConfig.contains("watchDog") && triggersConfig["watchDog"].isArray()){
                QJsonArray watchDogArray = triggersConfig["watchDog"].toArray();
                for(const auto& value: watchDogArray){
                    watchDogTriggers.append(value);
                }
            }
        }

        triggers["ping"] = pingTriggers;
        triggers["diskSpace"] = diskSpaceTriggers;
        triggers["watchDog"] = watchDogTriggers;


        QJsonDocument jsonDoc(triggers);
        responseData = jsonDoc.toJson();
        contentType = "application/json";
    }
    else if (method == "GET" && path == "/operations")
    {
        QJsonArray operationsArray;
        if(config.contains("operations") && config["operations"].isArray()){
            operationsArray = config["operations"].toArray();
        }
        QJsonDocument jsonDoc(operationsArray);
        responseData = jsonDoc.toJson();
        contentType = "application/json";
    }
    else if (method == "POST" && path == "/system")
    {


        qDebug() << getFormattedDateTime() << "Received system info:\n" << bodyData;
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            qDebug() << getFormattedDateTime() << "Error: Could not parse system info: " << jsonError.errorString();
            socket->disconnectFromHost();
            return;
        }
        if (!jsonDoc.isObject()) {
            qDebug() << getFormattedDateTime() << "Error: system info is not a JSON object";
            socket->disconnectFromHost();
            return;
        }
        QJsonObject systemInfo = jsonDoc.object();
        if(!systemInfo.contains("username") || !systemInfo.contains("hostname")) {
            qDebug() << getFormattedDateTime() << "Error: system info does not contain username or hostname";
            socket->disconnectFromHost();
            return;
        }

        QString username = systemInfo["username"].toString();
        QString hostname = systemInfo["hostname"].toString();
        AuthAgent authAgent{username, hostname};
        if(!allowedAgents.contains(authAgent)){
            qDebug() << getFormattedDateTime() << "Error: agent " << username << "@" << hostname << " is not allowed";
            socket->disconnectFromHost();
            return;
        }

        QJsonObject json;
        json["message"] = "System info received by server (Qt)";
        QJsonDocument jsonDocResponse(json);
        responseData = jsonDocResponse.toJson();
        contentType = "application/json";
    }
    else if (method == "POST" && path == "/message")
    {
        // Обработка POST запроса.

        qDebug() << getFormattedDateTime() << "Post request body:\n" <<  bodyData;
        QJsonObject json;
        json["message"] = "Message received by server (Qt)";
        QJsonDocument jsonDoc(json);
        responseData = jsonDoc.toJson();
        contentType = "application/json";
    }else if (method == "POST" && path == "/operationResult")
    {
        qDebug() << getFormattedDateTime() << "Received operation result:\n" <<  bodyData;

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(bodyData, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            qDebug() << getFormattedDateTime() << "Error: Could not parse operation result: " << jsonError.errorString();
            socket->disconnectFromHost();
            return;
        }
        if (!jsonDoc.isObject()) {
            qDebug() << getFormattedDateTime() << "Error: operation result is not a JSON object";
            socket->disconnectFromHost();
            return;
        }

        QJsonObject operationResult = jsonDoc.object();

        qDebug() << getFormattedDateTime() << "Operation result" << operationResult;

        QJsonObject json;
        json["message"] = "Operation result received by server (Qt)";
        QJsonDocument jsonDocResponse(json);
        responseData = jsonDocResponse.toJson();
        contentType = "application/json";
    }
    else {
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
