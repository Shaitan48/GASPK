// main.cpp (Agent Entry Point)
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "Agent main is running.";
    return app.exec();
}
