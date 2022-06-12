#ifndef DATABASE_H
#define DATABASE_H

#include"headers.h"

class Database
{
private:
    QSqlDatabase db;

public:
    void connect()//连接数据库
    {
        db = QSqlDatabase::addDatabase("QODBC");
        QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access}; DBQ=%1")\
                .arg(QCoreApplication::applicationDirPath() +"/InfoTable.accdb");//当前路径下
        db.setDatabaseName(dsn);//D:\\QT5\\aproject\\bookmanager\\library\\p
        db.open();//access数据库连接
    }
    bool isConnect()//检查是否连接成功
    {
        return db.isOpen();
    }
    QSqlDatabase getDB()//返回数据库
    {
        return db;
    }
    QString error()//返回出错的信息
    {
        return db.lastError().databaseText();
    }
};





#endif // DATABASE_H
