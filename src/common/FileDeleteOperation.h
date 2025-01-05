#ifndef FILEDELETEOPERATION_H
#define FILEDELETEOPERATION_H
#include "Operation.h"
class FileDeleteOperation : public Operation
{
 Q_OBJECT
public:
 explicit FileDeleteOperation(QObject *parent = nullptr);
   ~FileDeleteOperation() override = default;
  QJsonObject execute(const QJsonObject& params) override;
  QString getName() override;
  QString getId() override;
};

#endif // FILEDELETEOPERATION_H