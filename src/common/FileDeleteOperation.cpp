#include "FileDeleteOperation.h"
#include <QDebug>
#include <QFile>

FileDeleteOperation::FileDeleteOperation(qlonglong id, const QJsonObject &parameters, QObject *parent) : Operation(id, parent), m_parameters(parameters)
{

}
QJsonObject FileDeleteOperation::parameters() const
{
    return m_parameters;
}
QString FileDeleteOperation::name() const {
    return "fileDelete";
}
void FileDeleteOperation::execute() {
    if(m_parameters.contains("path") && m_parameters["path"].isString()){
        QString path = m_parameters["path"].toString();
        QFile file(path);
        if(file.exists()){
            if(file.remove()){
                qDebug() << "File " + path + " deleted successfully";
            } else {
                qDebug() << "Could not delete file " + path;
            }
        } else {
            qDebug() << "File " + path + " does not exist";
        }
    } else {
        qDebug() << "Path parameter is not found";
    }
}
