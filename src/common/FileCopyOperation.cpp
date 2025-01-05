#include "FileCopyOperation.h"
#include <QFile>
#include <QDebug>
#include <QUuid>

FileCopyOperation::FileCopyOperation(QObject *parent) : Operation(parent)
{

}

QJsonObject FileCopyOperation::execute(const QJsonObject &params)
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
      if(sourceFile.copy(destination)){
          result["status"] = "success";
            result["message"] = "File " + source + " copied to " + destination + " successfully";
      }
       else{
         result["status"] = "error";
            result["message"] = "Could not copy file " + source + " to " + destination;
      }
    return result;
}

QString FileCopyOperation::getName() {
    return "fileCopy";
}

QString FileCopyOperation::getId()
{
    return QUuid::createUuid().toString();
}