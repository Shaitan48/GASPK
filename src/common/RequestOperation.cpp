#include "RequestOperation.h"
#include <QDebug>
#include <QUuid>

RequestOperation::RequestOperation(QObject *parent) : Operation(parent)
{

}

QJsonObject RequestOperation::execute(const QJsonObject &params)
{
      QJsonObject result;
    if(!params.contains("text") || !params["text"].isString()){
        result["status"] = "error";
         result["message"] = "Text parameter not found";
            return result;
     }
   QString text = params["text"].toString();
     qDebug() << "Request operation " << text;
     result["status"] = "success";
      result["message"] = "Request was successfully sent";
     result["user_action_required"] = true;
      return result;
}

QString RequestOperation::getName() {
    return "request";
}

QString RequestOperation::getId()
{
    return QUuid::createUuid().toString();
}