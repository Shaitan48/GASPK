#include "Agent.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStorageInfo>
#include <QProcess>
#include <QDirIterator>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>

Agent::Agent(QObject *parent) : QObject(parent), timer(new QTimer(this)), manager(new QNetworkAccessManager(this))
{
    serverUrl = "http://localhost:1234/system";
    connect(timer, &QTimer::timeout, this, &Agent::sendSystemInfo);
    connect(manager, &QNetworkAccessManager::finished, this, &Agent::onReply);
    timer->start(5000); // Send data every 5 seconds
}

void Agent::sendSystemInfo()
{
    QJsonObject systemInfo;
    // Disk space info
    QJsonArray diskInfoArray;
    for (const auto& storage : QStorageInfo::mountedVolumes())
    {
        if(storage.isValid() && !storage.isReadOnly()){
            QJsonObject diskInfo;
            diskInfo["path"] = storage.rootPath();
            diskInfo["bytes_free"] = storage.bytesFree();
            diskInfo["bytes_total"] = storage.bytesTotal();
            diskInfoArray.append(diskInfo);
        }
    }
    systemInfo["disk_info"] = diskInfoArray;

    // Process list
    QJsonArray processList;
    QProcess process;
#ifdef Q_OS_WIN
    process.start("tasklist", QStringList() << "/fo" << "csv" << "/nh");
    if (!process.waitForFinished())
        return;
    QByteArray processListBytes = process.readAllStandardOutput();
    QString processListString = QString::fromUtf8(processListBytes);
    QStringList processLines = processListString.split('\n');
    for (const QString& line : processLines) {
        QStringList parts = line.split("\",\"");
        if(parts.size() > 1){
            QJsonObject processInfo;
            processInfo["name"] = parts[0].remove('"');
            processInfo["pid"] = parts[1].remove('"');
            processList.append(processInfo);
        }

    }

#else
    process.start("ps", QStringList() << "-e" << "-o" << "comm,pid");
    if(!process.waitForFinished())
        return;

    QByteArray processListBytes = process.readAllStandardOutput();
    QString processListString = QString::fromUtf8(processListBytes);
    QStringList processLines = processListString.split('\n');

    for (const QString& line : processLines) {
        QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(parts.size() > 1){
            QJsonObject processInfo;
            processInfo["name"] = parts[0];
            processInfo["pid"] = parts[1];
            processList.append(processInfo);
        }
    }
#endif
    systemInfo["processes"] = processList;


    // File monitoring
    QJsonArray fileList;
    QDirIterator it("/", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        if (it.fileInfo().fileName().contains("test")) {
            QJsonObject fileInfo;
            fileInfo["path"] = it.filePath();
            fileInfo["created"] = it.fileInfo().birthTime().toString();
            fileList.append(fileInfo);
        }
    }
    systemInfo["files"] = fileList;

    QJsonDocument doc(systemInfo);
    QByteArray jsonData = doc.toJson();
    qDebug() << "Sending system info:\n" << jsonData;

    QNetworkRequest request((QUrl(serverUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager->post(request, jsonData);
}

void Agent::onReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        qDebug() << "Response from server:\n" << response;
    }
    else
    {
        qDebug() << "Error from server:\n" << reply->errorString();
    }
    reply->deleteLater();
}
