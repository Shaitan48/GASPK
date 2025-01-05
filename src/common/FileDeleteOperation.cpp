#include "FileDeleteOperation.h"
#include <QFile>
#include <QDebug>
#include <QUuid>
FileDeleteOperation::FileDeleteOperation(QObject *parent) : Operation(parent)
{

}
QJsonObject FileDeleteOperation::execute(const QJsonObject &params)
{
 QJsonObject result;
 if(!params.contains("path") || !params["path"].isString()){
   result["status"] = "error";
   result["message"] = "Path parameter is not found";
     return result;
 }
 QString path = params["path"].toString();
   QFile file(path);
   if(file.exists()){
       if(file.remove()){
              result["status"] = "success";
               result["message"] = "File " + path + " deleted successfully";
       }else{
          result["status"] = "error";
          result["message"] = "Could not delete file " + path;
       }
   }else{
       result["status"] = "error";
        result["message"] = "File " + path + " does not exist";
   }

 return result;
}
QString FileDeleteOperation::getName() {
 return "fileDelete";
}
QString FileDeleteOperation::getId()
{
 return QUuid::createUuid().toString();
}