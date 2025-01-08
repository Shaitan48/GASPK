#include "Agent.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Agent agent;
    agent.start();

    return app.exec();
}
