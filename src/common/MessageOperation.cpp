#include "MessageOperation.h"
#include <QDebug>
#include <QUuid>
MessageOperation::MessageOperation(QObject *parent) : Operation(parent)
{

}
QJsonObject MessageOperation::execute(const QJsonObject &params)
{
    QJsonObject result;
    if(!params.contains("text") || !params["text"].isString()){
       result["status"] = "error";
       result["message"] = "Text parameter not found";
        return result;
    }
   QString text = params["text"].toString();
    qDebug() << "Message operation: " << text;
     result["status"] = "success";
    result["message"] = "Message was successfully shown";
    return result;
}
QString MessageOperation::getName() {
    return "message";
}
QString MessageOperation::getId()
{
    return QUuid::createUuid().toString();
}