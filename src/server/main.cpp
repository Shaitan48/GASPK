// main.cpp
#include "Server.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Server server;
    if (!server.startServer(8080)) {
        qDebug() << "Failed to start the server.";
        return -1;
    }

    qDebug() << "Server is running on port 8080.";
    return app.exec();
}
