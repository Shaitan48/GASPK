// src/common/RequestOperation.cpp
#include "RequestOperation.h"
#include <QDebug>

RequestOperation::RequestOperation(qlonglong id, const QJsonObject &parameters, QObject *parent) : Operation(id, parent), m_parameters(parameters)
{
}

QJsonObject RequestOperation::parameters() const
{
    return m_parameters;
}

QString RequestOperation::name() const
{
    return "request";
}

void RequestOperation::execute()
{
    if (m_parameters.contains("text") && m_parameters["text"].isString()) {
        QString text = m_parameters["text"].toString();
        qDebug() << "Requesting: " << text;
        // Добавьте вашу логику выполнения запроса здесь
    } else {
        qDebug() << "Request operation: No text parameter found.";
    }
}
