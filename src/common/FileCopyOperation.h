#ifndef FILECOPYOPERATION_H
#define FILECOPYOPERATION_H
#include "Operation.h"
class FileCopyOperation : public Operation
{
 Q_OBJECT
public:
 explicit FileCopyOperation(QObject *parent = nullptr);
   ~FileCopyOperation() override = default;
  QJsonObject execute(const QJsonObject& params) override;
   QString getName() override;
   QString getId() override;
};

#endif // FILECOPYOPERATION_H