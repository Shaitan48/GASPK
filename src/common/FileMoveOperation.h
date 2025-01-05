#ifndef FILEMOVEOPERATION_H
#define FILEMOVEOPERATION_H
#include "Operation.h"
class FileMoveOperation : public Operation
{
    Q_OBJECT
public:
    explicit FileMoveOperation(QObject *parent = nullptr);
      ~FileMoveOperation() override = default;
     QJsonObject execute(const QJsonObject& params) override;
      QString getName() override;
      QString getId() override;
};

#endif // FILEMOVEOPERATION_H