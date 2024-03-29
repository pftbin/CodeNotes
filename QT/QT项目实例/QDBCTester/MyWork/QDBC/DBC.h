#ifndef DBC_H
#define DBC_H

#include <QObject>
#include "IDBC.h"
#include "QDBObject.h"

class DBC : public QObject, public IDBC
{
    Q_OBJECT

public:
    DBC();
    ~DBC();

public:
    /***************************初始化*******************************/
    virtual bool InitDBServer(const QString &ip, const QString &port, const QString &dbname, const QString &username, const QString &password);
    virtual bool UnInitDBServer();

    /***************************测试函数*****************************/
    virtual bool SelectAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText);
    virtual bool UpdateAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText);

    virtual bool InsertNewItem();
    virtual bool DeleteItem();
    virtual bool UpdateItem();

protected:
    void ReConnetCheck();

private:
    QString         m_ip;
    QString         m_port;
    QString         m_dbname;
    QString         m_username;
    QString         m_password;
    QDBObject       m_dbObject;

};

#endif // DBC_H
