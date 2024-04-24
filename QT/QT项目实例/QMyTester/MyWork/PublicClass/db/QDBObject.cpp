#include "QDBObject.h"

#include <QCoreApplication>
#include <QDir>

#include "QLogger.h"
IMPLEMENT_LOGER(logger_QDBC);

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
        WRITE_LOG(logger_QDBC,QString(tr("QueryThread start...")),false);
        if(m_pParent)
        {
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doQuery();
            runSqlMutex.unlock();
        }
        WRITE_LOG(logger_QDBC,QString(tr("QueryThread end...")),false);
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
        WRITE_LOG(logger_QDBC,QString(tr("ExecuteThread start...")),false);
        if(m_pParent)
        {
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doExecute();
            runSqlMutex.unlock();
        }
        WRITE_LOG(logger_QDBC,QString(tr("ExecuteThread end...")),false);
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
        WRITE_LOG(logger_QDBC,QString(tr("CommitThread start...")),false);
        if(m_pParent)
        {
            //直接访问主线程变量【需注意访问共享资源】
            runSqlMutex.lock();
            m_pParent->doCommit();
            runSqlMutex.unlock();
        }
        WRITE_LOG(logger_QDBC,QString(tr("CommitThread end...")),false);
    }

};

/////////////////////////////////////////////////////////////////////////////////////

#include <QRandomGenerator>
static QString getRandomString(int maxLen)
{
    QRandomGenerator random(QRandomGenerator::global()->generate());
    QString result;
    int length = random.bounded(maxLen) + 1; // 随机生成字符串长度，范围为1-maxLen
    for (int i = 0; i < length; ++i)
    {
        QChar randomChar = QChar('a' + random.bounded('z' - 'a' + 1)); // 生成随机字符，范围为a-z
        if (random.bounded(2) == 0)
        {
            randomChar = randomChar.toUpper(); // 50%的概率将字符转换为大写
        }
        result.append(randomChar);
    }

    return result;
}

#include <QDateTime>
#include <QElapsedTimer>
static qint64 getTickCount()
{
    return QDateTime::currentMSecsSinceEpoch();
}
/////////////////////////////////////////////////////////////////////////////////////

QDBObject::QDBObject(const QString &ip, const QString &port, const QString &dbname,
                     const QString &username, const QString &password)
{
    QString strDirName = QString("%1/logPath/QDBCLog").arg(QCoreApplication::applicationDirPath());
    INIT_LOGER(logger_QDBC, QString("QDBC.log"),strDirName);

    m_strConnectName = QString("QDBObject_%1").arg(getRandomString(16));
    m_db = QSqlDatabase::addDatabase("QODBC", m_strConnectName);
    m_db.setHostName(ip);
    m_db.setPort(port.toInt());
    m_db.setDatabaseName(dbname);
    m_db.setUserName(username);
    m_db.setPassword(password);

    QObject::connect(&m_timerCheckConnect, &QTimer::timeout, this, &QDBObject::on_timerCheckConnect);
    m_timerCheckConnect.start(1000);
}

QDBObject::QDBObject()
{
    QString strDirName = QString("%1/logPath/QDBCLog").arg(QCoreApplication::applicationDirPath());
    INIT_LOGER(logger_QDBC, QString("QDBC.log"),strDirName);
}

QDBObject::~QDBObject()
{
    closeDatabase();
}

void QDBObject::setDBParam(const QString &ip, const QString &port, const QString &dbname,
                           const QString &username, const QString &password)
{
    m_strConnectName = QString("QDBObject_%1").arg(getRandomString(16));
    m_db = QSqlDatabase::addDatabase("QODBC", m_strConnectName);
    m_db.setHostName(ip);
    m_db.setPort(port.toInt());
    m_db.setDatabaseName(dbname);
    m_db.setUserName(username);
    m_db.setPassword(password);

    QObject::connect(&m_timerCheckConnect, &QTimer::timeout, this, &QDBObject::on_timerCheckConnect);
    m_timerCheckConnect.start(1000);
}

bool QDBObject::isOpen()
{
    return m_db.isOpen();
}
bool QDBObject::openDatabase()
{
    bool bResult = false;
    if (m_db.isOpen() || m_db.open())
    {
        bResult = true; // 数据库打开成功
    }
    else
    {
        QString strLogInfo = QString(tr("Open DataBase Failed: %1")).arg(m_db.lastError().text());
        WRITE_LOG(logger_QDBC, strLogInfo,false);
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
QString QDBObject::getLastError()
{
    return m_strLastError;
}

bool QDBObject::querySql(const QString &sql, QSqlQuery &query)
{
    runSqlMutex.lock();
    m_strSQL = sql;
    qint64 dwS = getTickCount();
    bool bQueryResult = doQuery();
    qint64 dwE = getTickCount();
    if (bQueryResult)
        query = std::move(m_sqlQueryObject);
    runSqlMutex.unlock();

    QString strResult = (!bQueryResult)?(tr("Query Failed")):(tr("Query Success"));
    QString strLogInfo = QString(tr("%1[%2 ms]: %3")).arg(strResult).arg(dwE-dwS).arg(sql);
    if (!bQueryResult) strLogInfo += QString("[%1]").arg(m_strLastError);
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return bQueryResult;
}
bool QDBObject::executeSql(const QString &sql)
{
    runSqlMutex.lock();
    m_strSQL = sql;
    qint64 dwS = getTickCount();
    bool bExecuteResult = doExecute();
    qint64 dwE = getTickCount();
    runSqlMutex.unlock();

    QString strResult = (!bExecuteResult)?(tr("Execute Failed")):(tr("Execute Success"));
    QString strLogInfo = QString(tr("%1[%2 ms]: %3")).arg(strResult).arg(dwE-dwS).arg(sql);
    if (!bExecuteResult) strLogInfo += QString("[%1]").arg(m_strLastError);
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return bExecuteResult;
}
bool QDBObject::commitSql(const QVector<QString> &vecSql)
{
    runSqlMutex.lock();
    m_vecSQL.clear();
    for(int i = 0; i < vecSql.size(); i++){
        m_vecSQL.append(vecSql.at(i));
    }
    qint64 dwS = getTickCount();
    bool bCommitResult = doCommit();
    qint64 dwE = getTickCount();
    runSqlMutex.unlock();

    QString strResult = (!bCommitResult)?(tr("Commit Failed")):(tr("Commit Success"));
    QString strLogInfo = QString(tr("%1[%2 ms]...")).arg(strResult).arg(dwE-dwS);
    if (!bCommitResult) strLogInfo += QString("[%1]").arg(m_strLastError);
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return bCommitResult;
}

//////////////////////////////////////////////////////////////////////////////
bool QDBObject::doQuery()
{
    bool bResult = false;
    try
    {
        if (!m_db.isOpen()) {
            WRITE_LOG(logger_QDBC, QString(tr("DataBase is close...")), false);
            return bResult;
        }

        if (m_strSQL.isEmpty())
        {
            WRITE_LOG(logger_QDBC, QString(tr("SQL is empty...")), false);
            return bResult;
        }

        int nReTry = 0;
        while (nReTry < 3)
        {
            WRITE_LOG(logger_QDBC, QString(tr("doQuery Retry=%1: %2")).arg(nReTry+1).arg(m_strSQL), false);
            m_sqlQueryObject = QSqlQuery(m_db);
            if (m_sqlQueryObject.exec(m_strSQL))
            {
                bResult = true;
                break;
            }
            nReTry++;
        }

        if (!bResult)
        {
            QSqlError error = m_sqlQueryObject.lastError();
            m_strLastError = QString(tr("Error: Type=%1, %2")).arg(error.type()).arg(error.text());
        }
    }
    catch (const std::exception &e)
    {
        bResult = false;
        QString strError = QString::fromStdString(e.what());
        WRITE_LOG(logger_QDBC, QString(tr("doQuery catch: %1")).arg(strError), false);
        return false;
    }

    emit queryFinished(); // query完成信号
    return bResult;
}
bool QDBObject::doExecute()
{
    bool bResult = false;
    try
    {
        if (!m_db.isOpen()) {
            WRITE_LOG(logger_QDBC, QString(tr("DataBase is close...")), false);
            return bResult;
        }

        if (m_strSQL.isEmpty())
        {
            WRITE_LOG(logger_QDBC, QString(tr("SQL is empty...")), false);
            return bResult;
        }

        int nReTry = 0;
        while (nReTry < 3)
        {
            WRITE_LOG(logger_QDBC, QString(tr("doExecute Retry=%1: %2")).arg(nReTry+1).arg(m_strSQL), false);
            m_sqlQueryObject = QSqlQuery(m_db);
            if (m_sqlQueryObject.exec(m_strSQL))
            {
                bResult = true;
                break;
            }
            nReTry++;
        }
    }
    catch (const std::exception &e)
    {
        bResult = false;
        QString strError = QString::fromStdString(e.what());
        WRITE_LOG(logger_QDBC, QString(tr("doExecute catch: %1")).arg(strError), false);
        return false;
    }

    emit executeFinished(); // execute完成信号
    return bResult;
}
bool QDBObject::doCommit()
{
    bool bResult = false;
    try
    {
        if (!m_db.isOpen()) {
            WRITE_LOG(logger_QDBC, QString(tr("DataBase is close...")), false);
            return bResult;
        }

        if (m_vecSQL.isEmpty())
        {
            WRITE_LOG(logger_QDBC, QString(tr("SQLVector is empty...")), false);
            return bResult;
        }

        if (m_db.transaction())     //开启事务
        {
            bool bCommit = true;
            m_sqlQueryObject = QSqlQuery(m_db);
            for(int i = 0; i < m_vecSQL.size(); i++)
            {
                QString strSQL = m_vecSQL.at(i);
                if (strSQL.isEmpty()) continue;

                bool bExec = m_sqlQueryObject.exec(strSQL);
                WRITE_LOG(logger_QDBC, QString(tr("doCommit Result=%1: %2")).arg(bExec).arg(m_strSQL), false);
            }

            if (bCommit && m_db.commit())     //提交事务
            {
                bResult = true;
            }
            else
            {
                m_db.rollback();    //回滚事务
            }
        }
    }
    catch (const std::exception &e)
    {
        bResult = false;
        QString strError = QString::fromStdString(e.what());
        WRITE_LOG(logger_QDBC, QString(tr("doCommit catch: %1")).arg(strError), false);
    }

    emit commitFinished(); // commit完成信号
    return bResult;
}
void QDBObject::on_timerCheckConnect()
{
    openDatabase();
}

