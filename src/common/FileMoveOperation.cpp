#include "FileMoveOperation.h"
#include <QDebug>
#include <QFile>

FileMoveOperation::FileMoveOperation(qlonglong id, const QJsonObject &parameters, QObject *parent) : Operation(id, parent), m_parameters(parameters)
{

}
QJsonObject FileMoveOperation::parameters() const
{
    return m_parameters;
}
QString FileMoveOperation::name() const {
    return "fileMove";
}
void FileMoveOperation::execute(){
    if(!m_parameters.contains("source") || !m_parameters["source"].isString() || !m_parameters.contains("destination") || !m_parameters["destination"].isString()){
        qDebug() << "Source or destination parameter not found";
    } else {
        QString source = m_parameters["source"].toString();
        QString destination = m_parameters["destination"].toString();
        QFile sourceFile(source);
        QFile destinationFile(destination);
        if(!sourceFile.exists()){
            qDebug() << "Source file " + source + " does not exist";
        } else if(destinationFile.exists()){
            qDebug() << "Destination file " + destination + " exists";
        } else if(sourceFile.rename(destination)){
            qDebug() << "File " + source + " moved to " + destination + " successfully";
        } else {
            qDebug() << "Could not move file " + source + " to " + destination;
        }
    }
}
