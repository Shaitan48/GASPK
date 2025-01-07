#include <QCoreApplication>
#include "Agent.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Agent agent;
    return a.exec();
}
