#ifndef QDBOBJECT_H
#define QDBOBJECT_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QVector>

static QMutex runSqlMutex;//全局互斥量
class QDBObject : public QObject
{
    Q_OBJECT

    friend class QueryThread;
    friend class ExecuteThread;
    friend class CommitThread;

public:
    explicit QDBObject(const QString &ip, const QString &port, const QString &dbName,
                       const QString &username, const QString &password);
    QDBObject();
    ~QDBObject();

    void setDBParam(const QString &ip, const QString &port, const QString &dbName,
                    const QString &username, const QString &password);

    bool isOpen();
    bool openDatabase();
    void closeDatabase();

    bool querySql(const QString &sql, QSqlQuery &query);
    bool executeSql(const QString &sql);
    bool commitSql(const QVector<QString> &vecSql);

private slots:
    void doQuery();
    void doExecute();
    void doCommit();

signals:
    void queryFinished();
    void executeFinished();
    void commitFinished();

private:
    QString             m_strSQL;
    QVector<QString>    m_vecSQL;

    QSqlQuery       m_sqlQueryObject;
    bool            m_bQueryResult;
    bool            m_bExecuteResult;
    bool            m_bCommitResult;
    QSqlDatabase    m_db;
    QString         m_strConnectName;
};

class QueryThread : public QThread
{
    Q_OBJECT

public:
    QueryThread(QDBObject* parent) : m_pParent(parent) {}

private:
    QDBObject* m_pParent;

    // 线程执行函数
    void run() override
    {
        qDebug() << "QueryThread start...";
        if(m_pParent)
        {
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doQuery();
            runSqlMutex.unlock();
        }
        qDebug() << "QueryThread end...";
    }

};

class ExecuteThread : public QThread
{
    Q_OBJECT

public:
    ExecuteThread(QDBObject* parent) : m_pParent(parent) {}

private:
    QDBObject* m_pParent;

    // 线程执行函数
    void run() override
    {
        qDebug() << "ExecuteThread start...";
        if(m_pParent)
        {
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doExecute();
            runSqlMutex.unlock();
        }
        qDebug() << "ExecuteThread end...";
    }

};

class CommitThread : public QThread
{
    Q_OBJECT

public:
    CommitThread(QDBObject* parent) : m_pParent(parent) {}

private:
    QDBObject* m_pParent;

    // 线程执行函数
    void run() override
    {
        if(m_pParent)
        {
            qDebug() << "CommitThread start...";
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doCommit();
            runSqlMutex.unlock();
            qDebug() << "CommitThread end...";
        }
    }

};

#endif // QDBOBJECT_H

