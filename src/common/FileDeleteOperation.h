#ifndef FILEDELETEOPERATION_H
#define FILEDELETEOPERATION_H

#include "Operation.h"
#include <QJsonObject>

class FileDeleteOperation : public Operation
{
    Q_OBJECT
public:
    explicit FileDeleteOperation(qlonglong id, const QJsonObject &parameters, QObject *parent = nullptr);
    ~FileDeleteOperation() override = default;
    QJsonObject parameters() const;
    void execute() override;
    QString name() const override;

private:
    QJsonObject m_parameters;
};

#endif // FILEDELETEOPERATION_H
