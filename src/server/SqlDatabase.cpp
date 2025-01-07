#include "SqlDatabase.h"
#include <QDebug>
#include <QDir>
#include <QSqlError>
SqlDatabase::SqlDatabase(QObject *parent) : QObject{parent}
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::currentPath() + "/gaspk.db");
    if (!db.open())
        qDebug() << "Database not open: " + db.lastError().text();
    QSqlQuery query;
    if(query.exec("CREATE TABLE IF NOT EXISTS users (username VARCHAR(255) NOT NULL PRIMARY KEY, password VARCHAR(255) NOT NULL)"))
        qDebug() << "Table users created!";
    else
        qDebug() << "Table users not created: " + query.lastError().text();
    if(query.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER NOT NULL PRIMARY KEY, name VARCHAR(255), enabled BOOLEAN)"))
        qDebug() << "Table tasks created!";
    else
        qDebug() << "Table tasks not created: " + query.lastError().text();
}

SqlDatabase::~SqlDatabase()
{
    if(db.isOpen())
        db.close();
}

bool SqlDatabase::isOpen() const
{
    return db.isOpen();
}

bool SqlDatabase::open()
{
    if(!db.isOpen())
        return db.open();
    return true;
}

void SqlDatabase::close()
{
    if(db.isOpen())
        db.close();
}
