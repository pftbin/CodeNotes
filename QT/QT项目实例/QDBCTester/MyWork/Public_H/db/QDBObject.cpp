#include "QDBObject.h"

#include <QTimer>
#include <QEventLoop>

#define DF_TIMEOUT  (5000) //设置执行超时(ms)

QDBObject::QDBObject(const QString &ip, const QString &port, const QString &dbName,
                     const QString &username, const QString &password)
{
    m_strConnectName = "connect_DBObject_class";
    m_db = QSqlDatabase::addDatabase("QODBC", m_strConnectName);
    m_db.setHostName(ip);
    m_db.setPort(port.toInt());
    m_db.setDatabaseName(dbName);
    m_db.setUserName(username);
    m_db.setPassword(password);
}

QDBObject::QDBObject()
{
    m_db = QSqlDatabase::addDatabase("QODBC", m_strConnectName);
}

QDBObject::~QDBObject()
{
    closeDatabase();
}

void QDBObject::setDBParam(const QString &ip, const QString &port, const QString &dbName,
                           const QString &username, const QString &password)
{
    m_db.setHostName(ip);
    m_db.setPort(port.toInt());
    m_db.setDatabaseName(dbName);
    m_db.setUserName(username);
    m_db.setPassword(password);
}

bool QDBObject::isOpen()
{
    return m_db.isOpen();
}
bool QDBObject::openDatabase()
{
    bool bResult = false;
    if (m_db.open())
    {
        bResult = true; // 数据库打开成功
    }
    else
    {
        qDebug() << "打开数据库失败:" << m_db.lastError().text();
    }

    return bResult;
}

void QDBObject::closeDatabase()
{
    if (m_db.isOpen())
    {
        m_db.close();
        QSqlDatabase::removeDatabase(m_strConnectName);
    }
}

bool QDBObject::querySql(const QString &sql, QSqlQuery &query)
{
    //QMutexLocker locker(&runSqlMutex);
    //if (!runSqlMutex.tryLock(3000))//获取锁3秒超时
    //{
    //    qDebug() << "CommitThread get lock failed...";
    //    return;
    //}



    //runSqlMutex.lock();
    m_strSQL = sql;
    //runSqlMutex.unlock();

#if 0

    QTimer timer;
    QEventLoop loop;
    QThread* thread = new QThread(this);
    moveToThread(thread);

    // 连接信号和槽
    connect(thread, &QThread::started, this, &QDBObject::doQuery);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(this, &QDBObject::queryFinished, &loop, &QEventLoop::quit);
    connect(this, &QDBObject::queryFinished, thread, &QThread::quit);


    thread->start();

    // 设置超时
    timer.setSingleShot(true);
    timer.start(DF_TIMEOUT);

    // 等待线程执行完成或者定时器超时
    loop.exec();

#else

    QThread* thread = new QueryThread(this);
    moveToThread(thread);

    connect(this, &QDBObject::queryFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
    thread->wait();


#endif

    if (m_bQueryResult)
        query = std::move(m_sqlQueryObject);

    qDebug() << "query return...";
    return m_bQueryResult;
}

bool QDBObject::executeSql(const QString &sql)
{
    m_strSQL = sql;

    QThread* thread = new ExecuteThread(this);
    moveToThread(thread);

    connect(this, &QDBObject::executeFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
    thread->wait();

    qDebug() << "execute return...";
    return m_bExecuteResult;
}

bool QDBObject::commitSql(const QVector<QString> &vecSql)
{
    m_vecSQL.clear();
    for(int i = 0; i < vecSql.size(); i++){
        m_vecSQL.append(vecSql.at(i));
    }

    QThread* thread = new CommitThread(this);
    moveToThread(thread);

    connect(this, &QDBObject::commitFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
    thread->wait();

    qDebug() << "commit return...";
    return m_bCommitResult;
}

//////////////////////////////////////////////////////////////////////////////
void QDBObject::doQuery()
{
    m_bQueryResult = false;
    if (!m_db.isOpen()) {
        return;
    }

    int nReTry = 0;
    while (nReTry < 3)
    {
        m_sqlQueryObject = QSqlQuery(m_db);
        if (m_sqlQueryObject.exec(m_strSQL))
        {
            m_bQueryResult = true;
            break;
        }
        nReTry++;
    }

    if(m_bQueryResult)
    {
        qDebug() << "query success";
    }
    else
    {
        qDebug() << "query failed...";
    }

    emit queryFinished(); // query完成信号
}

void QDBObject::doExecute()
{
    m_bExecuteResult = false;
    if (!m_db.isOpen()) {
        return;
    }

    int nReTry = 0;
    while (nReTry < 3)
    {
        m_sqlQueryObject = QSqlQuery(m_db);
        if (m_sqlQueryObject.exec(m_strSQL))
        {
            m_bExecuteResult = true;
            break;
        }
        nReTry++;
    }

    if(m_bQueryResult)
    {
        qDebug() << "execute success";
    }
    else
    {
        qDebug() << "execute failed...";
    }

    emit executeFinished(); // execute完成信号
}
void QDBObject::doCommit()
{
    m_bCommitResult = false;
    if (!m_db.isOpen()) {
        return;
    }

    if (m_db.transaction())     //开启事务
    {
        bool bExec = true;
        m_sqlQueryObject = QSqlQuery(m_db);
        for(int i = 0; i < m_vecSQL.size(); i++)
        {
            QString strSQL = m_vecSQL.at(i);
            bExec &= m_sqlQueryObject.exec(strSQL);
        }

        if (bExec && m_db.commit())     //提交事务
        {
            m_bCommitResult = true;
        }
        else
        {
            m_db.rollback();    //回滚事务 
        }
    }

    emit commitFinished(); // commit完成信号
}

