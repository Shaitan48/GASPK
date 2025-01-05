#ifndef MESSAGEOPERATION_H
#define MESSAGEOPERATION_H
#include "Operation.h"
class MessageOperation : public Operation
{
    Q_OBJECT
public:
    explicit MessageOperation(QObject *parent = nullptr);
      ~MessageOperation() override = default;
     QJsonObject execute(const QJsonObject& params) override;
      QString getName() override;
      QString getId() override;
};

#endif // MESSAGEOPERATION_H