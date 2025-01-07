#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QString>
#include <QList>

class QTcpSocket;
class QNetworkAccessManager;
class QNetworkReply;
class PingTrigger;
class DiskSpaceTrigger;
class TriggerWatchDog;
class Trigger;
class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(QObject *parent = nullptr);
    ~Agent() override;
private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onReply(QNetworkReply* reply);
private:
    void loadConfig();
    void sendSystemInfo();
    void logMessage(const QString& message);
    bool shouldLog(const QString& level) const;
    QTcpSocket* tcpSocket;
    QNetworkAccessManager* manager;
    QString host;
    int port;
    QFile logFile;
    QString logLevel = "debug";
    QList<Trigger*> triggers;

};

#endif // AGENT_H
