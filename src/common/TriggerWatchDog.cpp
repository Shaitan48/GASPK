#include "TriggerWatchDog.h"
#include <QDebug>
#include <QJsonArray>

TriggerWatchDog::TriggerWatchDog(QObject *parent) : Trigger(parent)
{
}

TriggerWatchDog::~TriggerWatchDog()
{
    qDebug() << "TriggerWatchDog destroyed";
}

void TriggerWatchDog::addDirectory(const QString &path)
{
    directories.append(path);
}

void TriggerWatchDog::addRegex(const QString &regex)
{
    regexes.append(QRegularExpression(regex));
}

bool TriggerWatchDog::isTriggered(const QJsonObject &agentData)
{
    if (!agentData.contains("files") || !agentData["files"].isArray())
    {
        return false;
    }

    QJsonArray fileList = agentData["files"].toArray();
     for (const auto& value : fileList) {
        if(!value.isObject())
             continue;

         QJsonObject fileInfo = value.toObject();
       if(!fileInfo.contains("path") || !fileInfo.contains("created"))
           continue;
       QString path = fileInfo["path"].toString();
       for(const QString& dir : directories){
            if(path.startsWith(dir)){
              for(const QRegularExpression& regex: regexes)
               {
                   if(regex.match(path).hasMatch()){
                       qDebug() << "TriggerWatchDog: file match in " << dir << " with regex " << regex.pattern();
                      emit triggered(agentData);
                     return true;
                   }
               }
           }

       }
    }
    return false;
}