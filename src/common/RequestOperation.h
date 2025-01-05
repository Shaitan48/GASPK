// src/common/RequestOperation.h
#ifndef REQUESTOPERATION_H
#define REQUESTOPERATION_H

#include "Operation.h"
#include <QJsonObject>

class RequestOperation : public Operation
{
    Q_OBJECT
public:
    explicit RequestOperation(qlonglong id, const QJsonObject &parameters, QObject *parent = nullptr);
    ~RequestOperation() override = default;
    QJsonObject parameters() const;
    QString name() const override;
    void execute() override; // Добавили объявление

private:
    QJsonObject m_parameters;
};

#endif // REQUESTOPERATION_H
