// server.cpp
#include "Server.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>

Server::Server(QObject *parent)
    : QObject(parent)
{
    // Initialize the database connection
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("example.db");

    if (!database.open()) {
        qDebug() << "Failed to open the database:" << database.lastError().text();
    } else {
        qDebug() << "Database opened successfully.";
    }

    // Initialize the network manager
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &Server::onReplyFinished);
}

Server::~Server()
{
    if (database.isOpen()) {
        database.close();
    }
    delete networkManager;
}

bool Server::startServer(quint16 port)
{
    qDebug() << "Server started on port" << port;
    // Example: You can use this method to configure additional server settings if needed
    return true;
}

void Server::stopServer()
{
    qDebug() << "Server stopped.";
}

void Server::sendRequest(const QString &url)
{
    QNetworkRequest request(QUrl(url));
    networkManager->get(request);
}

void Server::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Reply received:" << reply->readAll();
    } else {
        qDebug() << "Error in reply:" << reply->errorString();
    }
    reply->deleteLater();
}
