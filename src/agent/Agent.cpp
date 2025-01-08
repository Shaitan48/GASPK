// Agent.cpp
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "Agent started.";
    // Add agent logic here
    return app.exec();
}
