#ifndef FILECOPYOPERATION_H
#define FILECOPYOPERATION_H

#include "Operation.h"
#include <QJsonObject>

class FileCopyOperation : public Operation
{
    Q_OBJECT
public:
    explicit FileCopyOperation(qlonglong id, const QJsonObject &parameters, QObject *parent = nullptr);
    ~FileCopyOperation() override = default;
    QJsonObject parameters() const;
    QString name() const override;
    void execute() override;

private:
    QJsonObject m_parameters;
};

#endif // FILECOPYOPERATION_H
