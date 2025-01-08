// Client.cpp
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "Client started.";
    // Add client logic here
    return app.exec();
}
