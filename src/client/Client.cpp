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

Client::Client(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)), manager(new QNetworkAccessManager(this)) {
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(manager, &QNetworkAccessManager::finished, this, &Client::onReply);

    sendSystemInfo();
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

void Client::onConnected() {
    qDebug() << "Connected to server.";
    sendMessage("Hello from client");
}

void Client::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received data from server: " << data;
}

void Client::onDisconnected() {
    qDebug() << "Disconnected from server.";
}

void Client::onReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response from server: " << response;
    }
    else
    {
        qDebug() << "Error from server: " << reply->errorString();
    }
    reply->deleteLater();
}
