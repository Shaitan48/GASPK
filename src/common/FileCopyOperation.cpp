#include "FileCopyOperation.h"
#include <QDebug>
#include <QFile>

FileCopyOperation::FileCopyOperation(qlonglong id, const QJsonObject &parameters, QObject *parent) : Operation(id, parent), m_parameters(parameters)
{

}
QJsonObject FileCopyOperation::parameters() const
{
    return m_parameters;
}
QString FileCopyOperation::name() const {
    return "fileCopy";
}
void FileCopyOperation::execute(){
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
        } else if(sourceFile.copy(destination)){
            qDebug() << "File " + source + " copied to " + destination + " successfully";
        } else {
            qDebug() << "Could not copy file " + source + " to " + destination;
        }
    }
}
