 #include "Operation.h"

Operation::Operation(qlonglong id, QObject *parent) : QObject(parent), m_id(id)
{

}
qlonglong Operation::id() const {
    return m_id;
}
