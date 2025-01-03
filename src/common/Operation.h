#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QJsonObject>

class Operation : public QObject
{
    Q_OBJECT

public:
    explicit Operation(QObject *parent = nullptr);
    virtual void execute(const QJsonObject& agentData) = 0;
    virtual ~Operation() = default;
};

#endif // OPERATION_H
