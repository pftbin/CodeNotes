#ifndef IDBC_H
#define IDBC_H

#include <QString>

class IDBC
{

public:
    /***************************初始化*******************************/
    virtual bool InitDBServer(const QString &ip, const QString &port, const QString &dbname, const QString &username, const QString &password) = 0;
    virtual bool UnInitDBServer() = 0;

    /***************************测试函数*****************************/
    virtual bool SelectAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText) = 0;
    virtual bool UpdateAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText) = 0;

    virtual bool InsertNewItem() = 0;
    virtual bool DeleteItem() = 0;
    virtual bool UpdateItem() = 0;

};

#endif // IDBC_H
