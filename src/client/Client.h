#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QString>
class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client() override;

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    void loadConfig();
    void logMessage(const QString& message);
    bool shouldLog(const QString& level) const;
    void sendSystemInfo();
    QTcpSocket* tcpSocket;
    QString host;
    int port;
    QFile logFile;
    QString logLevel = "debug";

};

#endif // CLIENT_H
