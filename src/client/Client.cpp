#include "Client.h"
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
#include <QJsonArray>
#include "MainWindow.h"

Client::Client(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)), manager(new QNetworkAccessManager(this)) {
    mainWindow = new MainWindow();
    mainWindow->show();

    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(manager, &QNetworkAccessManager::finished, this, &Client::onReply);
    connectToServer();
}

void Client::sendSystemInfo() {
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

void Client::connectToServer() {
    socket->connectToHost("localhost", 1234);
    if(!socket->waitForConnected(3000)) {
        qDebug() << "Error: " << socket->errorString();
        QCoreApplication::exit(1);
    }
}

void Client::sendMessage(const QString &message) {
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
void Client::loadOperations(){
    QUrl url("http://localhost:1234/operations");
    QNetworkRequest request(url);
    manager->get(request);
    qDebug() << "Sent get operations request";
}
void Client::onConnected() {
    qDebug() << "Connected to server.";
    sendSystemInfo();
}

void Client::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received data from server: " << data;
}

void Client::onDisconnected() {
    qDebug() << "Disconnected from server.";
    QCoreApplication::exit(0);
}

void Client::performOperation(const QJsonObject &operation) {
    QString operationName = operation["name"].toString();
    QJsonObject parameters = operation["parameters"].toObject();
    qDebug() << "Performing operation: " << operationName << " with params " << parameters;

    if (operationName == "fileDelete") {
        // Выполнение удаления файла
        if(parameters.contains("path") && parameters["path"].isString()){
            QString path = parameters["path"].toString();
            QFile file(path);
            if(file.exists()){
                if(file.remove()){
                    qDebug() << "File " + path + " deleted successfully";
                } else {
                    qDebug() << "Could not delete file " + path;
                }
            } else {
                qDebug() << "File " + path + " does not exist";
            }
        } else {
            qDebug() << "Path parameter is not found";
        }
    }
    else if (operationName == "fileCopy") {
        if(!parameters.contains("source") || !parameters["source"].isString() || !parameters.contains("destination") || !parameters["destination"].isString()){
            qDebug() << "Source or destination parameter not found";
        } else {
            QString source = parameters["source"].toString();
            QString destination = parameters["destination"].toString();
            QFile sourceFile(source);
            QFile destinationFile(destination);
            if(!sourceFile.exists()){
                qDebug() << "Source file " + source + " does not exist";
            } else if(destinationFile.exists()){
                qDebug() << "Destination file " + destination + " exists";
            } else if(sourceFile.copy(destination)){
                qDebug() << "File " + source + " copied to " + destination + " successfully";
            } else {
                qDebug() << "Could not copy file " + source + " to " + destination;
            }
        }
    }
    else if (operationName == "fileMove") {
        if(!parameters.contains("source") || !parameters["source"].isString() || !parameters.contains("destination") || !parameters["destination"].isString()){
            qDebug() << "Source or destination parameter not found";
        } else {
            QString source = parameters["source"].toString();
            QString destination = parameters["destination"].toString();
            QFile sourceFile(source);
            QFile destinationFile(destination);
            if(!sourceFile.exists()){
                qDebug() << "Source file " + source + " does not exist";
            } else if(destinationFile.exists()){
                qDebug() << "Destination file " + destination + " exists";
            } else if(sourceFile.rename(destination)){
                qDebug() << "File " + source + " moved to " + destination + " successfully";
            } else {
                qDebug() << "Could not move file " + source + " to " + destination;
            }
        }
    } else if (operationName == "message") {
        if(parameters.contains("text") && parameters["text"].isString()){
            QString text = parameters["text"].toString();
            qDebug() << text;
        } else {
            qDebug() << "Text parameter not found";
        }
    } else if (operationName == "request") {
        if(parameters.contains("text") && parameters["text"].isString()){
            QString text = parameters["text"].toString();
            qDebug() << "Request: " + text;
        }else{
            qDebug() << "Text parameter not found";
        }
    } else {
        qDebug() << "Operation " + operationName +  " is not supported by client";
    }
}

void Client::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response from server: " << response;

        if (!authorized && response.contains("System info received by server (Qt)")){
            sendMessage("Hello from client");
            authorized = true;
        } else if(authorized){
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &jsonError);
            if (jsonError.error != QJsonParseError::NoError) {
                qDebug() << "Error: Could not parse response: " << jsonError.errorString();
                return;
            }
            if (jsonDoc.isArray()){
                QJsonArray operationsArray = jsonDoc.array();
                QStringList operationStrings;
                for (const auto &operation : operationsArray) {
                    if (operation.isObject()) {
                        QJsonObject operationObject = operation.toObject();
                        QString operationName = operationObject["name"].toString();
                        operationStrings.append(operationName);
                    }
                }
                mainWindow->updateOperationList(operationStrings);
            }
        }
    }
    else
    {
        qDebug() << "Error from server: " << reply->errorString();
        socket->disconnectFromHost();
    }
    reply->deleteLater();
}
Client::~Client(){}
