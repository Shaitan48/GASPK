#ifndef FILEMOVEOPERATION_H
#define FILEMOVEOPERATION_H

#include "Operation.h"
#include <QJsonObject>

class FileMoveOperation : public Operation
{
    Q_OBJECT
public:
    explicit FileMoveOperation(qlonglong id, const QJsonObject &parameters, QObject *parent = nullptr);
    ~FileMoveOperation() override = default;
    QJsonObject parameters() const;
    QString name() const override;
    void execute() override;

private:
    QJsonObject m_parameters;
};

#endif // FILEMOVEOPERATION_H
