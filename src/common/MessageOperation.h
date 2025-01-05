#ifndef MESSAGEOPERATION_H
#define MESSAGEOPERATION_H

#include "Operation.h"
#include <QJsonObject>

class MessageOperation : public Operation
{
    Q_OBJECT
public:
    explicit MessageOperation(qlonglong id, const QJsonObject &parameters, QObject *parent = nullptr);
    ~MessageOperation() override = default;
    QJsonObject parameters() const;
    QString name() const override;
    void execute() override;

private:
    QJsonObject m_parameters;
};

#endif // MESSAGEOPERATION_H
