#include <QCoreApplication>
#include <QCommandLineParser>
#include "Agent.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Agent");
    parser.addHelpOption();

    QCommandLineOption serverUrlOption(QStringList() << "s" << "serverUrl", "Server URL", "serverUrl");
    QCommandLineOption intervalOption(QStringList() << "i" << "interval", "Interval", "interval");
    parser.addOption(serverUrlOption);
    parser.addOption(intervalOption);

    parser.process(app);

    QString serverUrl = parser.value(serverUrlOption);
    int interval = parser.value(intervalOption).toInt();

    Agent agent;


    qDebug() << "Agent started with server url:" << serverUrl << " and interval: " << interval;
    return app.exec();
}
