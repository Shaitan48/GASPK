#include "Agent.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QSqlError>
#include <QJsonParseError>

Agent::Agent(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &Agent::onReplyFinished);

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("agent.db");

    if (!database.open()) {
        qDebug() << "Failed to open the database:" << database.lastError().text();
    } else {
        qDebug() << "Database opened successfully.";
    }

    loadConfiguration();
}

Agent::~Agent()
{
    if (database.isOpen()) {
        database.close();
    }
    delete networkManager;
}

void Agent::start()
{
    qDebug() << "Agent started.";
    // Additional startup logic can go here
}

void Agent::stop()
{
    qDebug() << "Agent stopped.";
    // Additional cleanup logic can go here
}

void Agent::sendRequest(const QString &url)
{
    QNetworkRequest request((QUrl(url)));
    networkManager->get(request);
}

void Agent::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Reply received:" << reply->readAll();
    } else {
        qDebug() << "Error in reply:" << reply->errorString();
    }
    reply->deleteLater();
}

void Agent::loadConfiguration()
{
    QFile configFile("config.json");
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open config file.";
        return;
    }

    QByteArray data = configFile.readAll();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse config file:" << parseError.errorString();
        return;
    }

    QJsonObject config = jsonDoc.object();
    QString dbName = config["database"].toObject()["name"].toString();
    database.setDatabaseName(dbName);

    qDebug() << "Configuration loaded.";
}
