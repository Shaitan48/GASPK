#ifndef REQUESTOPERATION_H
#define REQUESTOPERATION_H
#include "Operation.h"
class RequestOperation : public Operation
{
    Q_OBJECT
public:
    explicit RequestOperation(QObject *parent = nullptr);
      ~RequestOperation() override = default;
     QJsonObject execute(const QJsonObject& params) override;
      QString getName() override;
      QString getId() override;
};

#endif // REQUESTOPERATION_H