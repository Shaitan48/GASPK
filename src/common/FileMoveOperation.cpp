#include "FileMoveOperation.h"
#include <QFile>
#include <QDebug>
#include <QUuid>

FileMoveOperation::FileMoveOperation(QObject *parent) : Operation(parent)
{

}

QJsonObject FileMoveOperation::execute(const QJsonObject &params)
{
    QJsonObject result;
    if(!params.contains("source") || !params["source"].isString() || !params.contains("destination") || !params["destination"].isString()){
      result["status"] = "error";
      result["message"] = "Source or destination parameter not found";
        return result;
    }
    QString source = params["source"].toString();
      QString destination = params["destination"].toString();
       QFile sourceFile(source);
       QFile destinationFile(destination);
       if(!sourceFile.exists()){
           result["status"] = "error";
           result["message"] = "Source file " + source + " does not exist";
              return result;
       }
          if(destinationFile.exists()){
              result["status"] = "error";
            result["message"] = "Destination file " + destination + " exists";
               return result;
         }
      if(sourceFile.rename(destination)){
           result["status"] = "success";
            result["message"] = "File " + source + " moved to " + destination + " successfully";
      }else{
         result["status"] = "error";
         result["message"] = "Could not move file " + source + " to " + destination;
      }
    return result;
}

QString FileMoveOperation::getName() {
    return "fileMove";
}

QString FileMoveOperation::getId()
{
    return QUuid::createUuid().toString();
}