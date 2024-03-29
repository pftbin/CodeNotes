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
    explicit QDBObject(const QString &ip, const QString &port, const QString &dbname,
                       const QString &username, const QString &password);
    QDBObject();
    ~QDBObject();

    void setDBParam(const QString &ip, const QString &port, const QString &dbname,
                    const QString &username, const QString &password);

    bool isOpen();
    bool openDatabase();
    void closeDatabase();
    QString getLastError();

    bool querySql(const QString &sql, QSqlQuery &query);
    bool executeSql(const QString &sql);
    bool commitSql(const QVector<QString> &vecSql);

private slots:
    bool doQuery();
    bool doExecute();
    bool doCommit();

signals:
    void queryFinished();
    void executeFinished();
    void commitFinished();

private:
    QString             m_strSQL;
    QVector<QString>    m_vecSQL;
    QSqlQuery           m_sqlQueryObject;

    QSqlDatabase        m_db;
    QString             m_strConnectName;
};

#endif // QDBOBJECT_H

