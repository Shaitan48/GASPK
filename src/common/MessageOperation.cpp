#include "MessageOperation.h"
#include <QDebug>

MessageOperation::MessageOperation(qlonglong id, const QJsonObject &parameters, QObject *parent) : Operation(id, parent), m_parameters(parameters)
{

}
QJsonObject MessageOperation::parameters() const
{
    return m_parameters;
}
QString MessageOperation::name() const {
    return "message";
}
void MessageOperation::execute(){
    if(m_parameters.contains("text") && m_parameters["text"].isString()){
        QString text = m_parameters["text"].toString();
        qDebug() << text;
    } else {
        qDebug() << "Text parameter not found";
    }
}
