#include <QCoreApplication>
#include "Agent.h"
#include <QCommandLineParser>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Agent for server monitoring");
    parser.addHelpOption();

    QCommandLineOption serverUrlOption(QStringList() << "s" << "server", "Server url", "url");
    parser.addOption(serverUrlOption);

    QCommandLineOption intervalOption(QStringList() << "i" << "interval", "Interval in milliseconds", "ms");
    parser.addOption(intervalOption);
    parser.process(app);
    QString serverUrl = "http://localhost:1234/system";
    if(parser.isSet(serverUrlOption)){
        serverUrl =  parser.value(serverUrlOption);
    }
    int interval = 5000;
    if(parser.isSet(intervalOption)){
        interval = parser.value(intervalOption).toInt();
    }


    qDebug() << "Starting Agent with server " << serverUrl << " and interval " << interval;

    Agent agent;
    agent.serverUrl = serverUrl;
    agent.timer->setInterval(interval);
    return app.exec();
}
