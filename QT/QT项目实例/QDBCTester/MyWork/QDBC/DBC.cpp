#include "DBC.h"
#include <QDebug>

#include "QLogger.h"
EXTERN_LOGER(logger_QDBC);
/////////////////////////////////////////////////////////////////////////////////////
//#include "public.h"
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
static int getRandomInt(int maxValue)
{
    QRandomGenerator random(QRandomGenerator::global()->generate());
    int randomNumber = random.bounded(maxValue+1); // 生成不超过maxValue的随机数

    return randomNumber;
}

#include <QVariant>
static QString ConvertToQString(const QVariant &variant)
{
    switch(variant.typeId())
    {
    case QMetaType::QString:
        return variant.toString().trimmed();
    case QMetaType::Int:
    case QMetaType::UInt:
        return QString::number(variant.toInt());
    case QMetaType::Double:
        return QString::number(variant.toDouble());
    // Add more cases here for other types that you want to convert
    default:
        return variant.toString().trimmed();
    }
}

/////////////////////////////////////////////////////////////////////////////////////

DBC::DBC()
{
    m_ip = "";
    m_port = "";
    m_dbname = "";
    m_username = "";
    m_password = "";
}
DBC::~DBC()
{
    UnInitDBServer();
}

void DBC::ReConnetCheck()
{
    if (!m_dbObject.isOpen())
        m_dbObject.openDatabase();
}
/////////////////////////////////////////////////////////////////////////

/***************************初始化*******************************/
bool DBC::InitDBServer(const QString &ip, const QString &port, const QString &dbname, const QString &username, const QString &password)
{
    if (ip==m_ip && port==m_port && dbname==m_dbname && username==m_username && password==m_password)
    {
        if (m_dbObject.isOpen())
        {
            return true;
        }
        else
        {
            if (m_dbObject.openDatabase())
            {
                return true;
            }
        }
    }
    else
    {
        m_ip = ip;
        m_port = port;
        m_dbname = dbname;
        m_username = username;
        m_password = password;
        m_dbObject.closeDatabase();

        m_dbObject.setDBParam(m_ip,m_port,m_dbname,m_username,m_password);
        if (m_dbObject.openDatabase())
        {
            return true;
        }
    }

    QString strLogInfo = QString("[InitDBServer]: IP=%1,PORT=%2,DBName=%3,User=%4,Password=%5, error=%6")
                         .arg(m_ip,m_port,m_dbname,m_username,m_password).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return false;
}
bool DBC::UnInitDBServer()
{
    if (m_dbObject.isOpen())
        m_dbObject.closeDatabase();

    return true;
}

/***************************测试函数*****************************/
bool DBC::SelectAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText)
{
    ReConnetCheck();

    bool bResult = false;
    QString strSelect = "select * from SYSDBA.SBT_TESTBY";
    QSqlQuery query;
    if (m_dbObject.querySql(strSelect, query))
    {
        bResult = true;
        QSqlRecord rec = query.record(); // 获取查询结果的元数据
        int columnCount = rec.count(); // 获取列数
        for (int i = 0; i < columnCount; i++)
        {
            QString strColName = rec.fieldName(i); // 获取列名
            vecColNameText.push_back(strColName);
        }

        while (query.next())
        {
            QVector<QString> vecRowItem;
            for (int nColIdx = 0; nColIdx < columnCount; nColIdx++)
            {
                QVariant value = query.value(nColIdx); // 获取每列的值
                QString strText = ConvertToQString(value);
                vecRowItem.push_back(strText);
            }
            if(!vecRowItem.isEmpty())
                vecRowItemText.append(vecRowItem);
        }
    }

    QString strLogInfo = QString("[SelectAllItem]: result=%1, error=%2").arg(bResult).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return bResult;
}
bool DBC::UpdateAllItem(QVector<QString>& vecColNameText, QVector<QVector<QString>>& vecRowItemText)
{
    ReConnetCheck();

    QVector<QString> vecSql;
    int nRowCount = vecRowItemText.size();
    int nColCount = vecColNameText.size();
    for (int nRowIdx = 0; nRowIdx < nRowCount; nRowIdx++)
    {
        QString strColText = "";
        QString strWhere = "";
        for (int nColIdx = 0; nColIdx < nColCount; nColIdx++)
        {
            QString strColName = vecColNameText[nColIdx];
            QString strItemValue = vecRowItemText[nRowIdx][nColIdx];

            if(strColName=="NAME" || strColName=="ADDRESS") //string need '
                strItemValue = QString("'%1'").arg(strItemValue);

            QString strItemText = QString("%1=%2").arg(strColName,strItemValue);
            if(nColIdx == 0)
                strWhere = strItemText;

            if (nColIdx != nColCount-1)
                strItemText += ",";
            strColText += strItemText;
        }

        if (!strColText.isEmpty() && !strWhere.isEmpty())
        {
            QString strSQL = QString("update SYSDBA.SBT_TESTBY set %1 where %2").arg(strColText,strWhere);
            vecSql.append(strSQL);
        }
    }

    bool bResult = false;
    if (m_dbObject.commitSql(vecSql))
    {
        bResult = true;
    }

    QString strLogInfo = QString("[UpdateAllItem]: result=%1, error=%2").arg(bResult).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);
    return bResult;
}
bool DBC::InsertNewItem()
{
    ReConnetCheck();

    int nID = 0;
    QString strSelect = "select max(ID) as COUNT from SYSDBA.SBT_TESTBY";
    QSqlQuery query;
    if (m_dbObject.querySql(strSelect, query))
    {
        if(query.first())
        {
            nID = query.value("COUNT").toInt();
            nID += 1;
        }
    }
    else
    {
        QString strLogInfo = QString("[InsertNewItem]: select max ID failed, lasterror=%1").arg(m_dbObject.getLastError());
        WRITE_LOG(logger_QDBC, strLogInfo, false);
        return false;
    }
    QString strName = getRandomString(10);
    int nAge = getRandomInt(100);
    QString strAddress = getRandomString(10);

    bool bResult = false;
    QString strSQL = QString("insert SYSDBA.SBT_TESTBY (ID,NAME,AGE,ADDRESS) values(%1,'%2',%3,'%4')").arg(nID).arg(strName).arg(nAge).arg(strAddress);
    bResult = m_dbObject.executeSql(strSQL);
    QString strLogInfo = QString("[InsertNewItem]: result=%1, error=%2").arg(bResult).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);

    return bResult;
}
bool DBC::DeleteItem()
{
    ReConnetCheck();

    int nID = 0;
    QString strSelect = "select max(ID) as COUNT from SYSDBA.SBT_TESTBY";
    QSqlQuery query;
    if (m_dbObject.querySql(strSelect, query))
    {
        if(query.first())
        {
            nID = query.value("COUNT").toInt();
        }
    }
    else
    {
        QString strLogInfo = QString("[DeleteItem]: select max ID failed, lasterror=%1").arg(m_dbObject.getLastError());
        WRITE_LOG(logger_QDBC, strLogInfo, false);
        return false;
    }

    bool bResult = false;
    QString strSQL = QString("delete from SYSDBA.SBT_TESTBY where ID=%1").arg(nID);
    bResult = m_dbObject.executeSql(strSQL);
    QString strLogInfo = QString("[DeleteItem]: result=%1, error=%2").arg(bResult).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);

    return bResult;
}
bool DBC::UpdateItem()
{
    ReConnetCheck();

    int nID = 0;
    QString strSelect = "select max(ID) as COUNT from SYSDBA.SBT_TESTBY";
    QSqlQuery query;
    if (m_dbObject.querySql(strSelect, query))
    {
        if(query.first())
        {
            nID = query.value("COUNT").toInt();
        }
    }
    else
    {
        QString strLogInfo = QString("[UpdateItem]: select max ID failed, lasterror=%1").arg(m_dbObject.getLastError());
        WRITE_LOG(logger_QDBC, strLogInfo, false);
        return false;
    }

    bool bResult = false;
    QString strSQL = QString("update SYSDBA.SBT_TESTBY set NAME='%1' where ID=%2").arg(getRandomString(10)).arg(nID);
    bResult = m_dbObject.executeSql(strSQL);
    QString strLogInfo = QString("[UpdateItem]: result=%1, error=%2").arg(bResult).arg(m_dbObject.getLastError());
    WRITE_LOG(logger_QDBC, strLogInfo, false);

    return bResult;
}

