#ifndef TRIGGERWATCHDOG_H
#define TRIGGERWATCHDOG_H

#include "Trigger.h"
#include <QList>
#include <QRegularExpression>
 #include <QJsonObject>

class TriggerWatchDog : public Trigger
{
    Q_OBJECT

public:
    TriggerWatchDog(QObject *parent = nullptr);
    ~TriggerWatchDog() override;
    void addDirectory(const QString &path);
    void addRegex(const QString &regex);
    bool isTriggered(const QJsonObject &agentData) override;

private:
    QList<QString> directories;
    QList<QRegularExpression> regexes;
};

#endif // TRIGGERWATCHDOG_H