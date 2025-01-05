#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QJsonObject>

class Operation : public QObject
{
    Q_OBJECT
public:
    explicit Operation(QObject *parent = nullptr);
    virtual ~Operation() = default;
    virtual QJsonObject execute(const QJsonObject& params) = 0;
    virtual QString getName() = 0;
    virtual QString getId() = 0;
};

#endif // OPERATION_H
