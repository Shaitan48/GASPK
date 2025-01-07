#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class SqlDatabase : public QObject
{
    Q_OBJECT

public:
    explicit SqlDatabase(QObject* parent = nullptr);
    ~SqlDatabase() override;
    bool isOpen() const;
    bool open();
    void close();
private:
    QSqlDatabase db;

};

#endif // SQLDATABASE_H
