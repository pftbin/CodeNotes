#ifndef QLOG_H
#define QLOG_H

#include "QLog_global.h"

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <QThread>

class QLOG_EXPORT QLog : public QObject
{
    Q_OBJECT

public:
    explicit QLog(QObject *parent = nullptr);
    virtual ~QLog();

    void SetLogParam(const QString& strFileName, const QString& strDirName = "");
    void writeLog(const QString& message, bool syncWrite = false);

protected:
    void resetLogFile();
    void writeToFile(const QString& message);

signals:
    void logWritten();

private slots:
    void processLogQueue();

private:
    bool              m_bParamInit;

    QString           m_strFileName;
    QString           m_strFilePrefix;

    QString           m_strFolderName;
    QString           m_strDirName;

    QFile             m_logFile;
    QTextStream       m_textStream;
    QMutex            m_mutex;
    QQueue<QString>   m_logQueue;
};

#define EXTERN_LOGER(logger) \
extern QLog logger(nullptr);

#define IMPLEMENT_LOGER(logger) \
QLog logger;

#define INIT_LOGER(logger,filename,folder) \
logger.SetLogParam(filename,folder);

#define WRITE_LOG(logger,message,sync) \
logger.writeLog(message,sync);

#endif // QLOG_H
