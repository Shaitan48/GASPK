#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QJsonObject>
#include <QString>
class Operation : public QObject
{
    Q_OBJECT
public:
    explicit Operation(qlonglong id, QObject *parent = nullptr);
    virtual ~Operation() = default;

    qlonglong id() const;
    virtual QString name() const = 0;
    virtual void execute() = 0;

private:
    qlonglong m_id;
};

#endif // OPERATION_H
