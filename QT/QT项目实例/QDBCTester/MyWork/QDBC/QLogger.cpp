#include "QLogger.h"
#include <QCoreApplication>
#include <QDir>

QLogger::QLogger(QObject *parent) : QObject(parent)
{
    m_bParamInit = false;
    connect(this, &QLogger::logWritten, this, &QLogger::processLogQueue, Qt::QueuedConnection);
}

QLogger::~QLogger()
{
    if (m_bParamInit)
    {
        if (!m_logQueue.isEmpty())
        {
            processLogQueue();
        }
        m_logFile.close();
    }
}

void QLogger::SetLogParam(const QString& strFileName, const QString& strDirName)
{
    m_strFileName = strFileName;
    m_strFilePrefix = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
    QString strFullFileName = QString("%1_%2").arg(m_strFilePrefix,m_strFileName);

    m_strFolderName = QDateTime::currentDateTime().toString("yyyyMMdd");
    m_strDirName = strDirName;
    if (strDirName.isEmpty())
        m_strDirName = QString("%1/logPath").arg(QCoreApplication::applicationDirPath());
    QString strFullDir = QString("%1/%2").arg(m_strDirName,m_strFolderName);
    QDir logDir(strFullDir);
    if (!logDir.exists())
        logDir.mkpath(".");

    QString strLogFilePath = logDir.filePath(strFullDir + "/" + strFullFileName);
    m_logFile.setFileName(strLogFilePath);
    m_logFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered);
    m_textStream.setDevice(&m_logFile);
    m_bParamInit = true;
}

void QLogger::writeLog(const QString& message, bool syncWrite)
{
    if (!m_bParamInit) return;

    QMutexLocker locker(&m_mutex);
    QString strTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString strMessage = QString("%1 %2\n").arg(strTime,message);
    if (syncWrite)
    {
        writeToFile(strMessage);
    }
    else
    {
        m_logQueue.enqueue(strMessage);
        emit logWritten();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void QLogger::resetLogFile()
{
    if (!m_bParamInit) return;

    bool bNewLogFile = false;
    QString strOldFullFileName = QString("%1_%2").arg(m_strFilePrefix,m_strFileName);
    QString strOldFullDir = QString("%1/%2").arg(m_strDirName,m_strFolderName);
    QString strOldLogFilePath = QString("%1/%2").arg(strOldFullDir,strOldFullFileName);
    QFile file(strOldLogFilePath);
    if (!file.exists())
        bNewLogFile = true;

    QString strNowDay = QDateTime::currentDateTime().toString("yyyyMMdd");
    if ((m_strFolderName != strNowDay) || bNewLogFile)
    {
        if (!m_logQueue.isEmpty())
        {
            processLogQueue();
        }
        m_logFile.close();

        //
        m_strFolderName = strNowDay;
        m_strFilePrefix = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm_ss");
        QString strFullFileName = QString("%1_%2").arg(m_strFilePrefix,m_strFileName);
        QString strFullDir = QString("%1/%2").arg(m_strDirName,m_strFolderName);
        QDir logDir(strFullDir);
        if (!logDir.exists())
            logDir.mkpath(".");
        QString strLogFilePath = logDir.filePath(strFullDir + "/" + strFullFileName);
        m_logFile.setFileName(strLogFilePath);
        m_logFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered);
        m_textStream.setDevice(&m_logFile);
    }
}

void QLogger::writeToFile(const QString& message)
{
    if (!m_bParamInit) return;

    resetLogFile();
    m_textStream << message;
    m_textStream.flush();
}

void QLogger::processLogQueue()
{
    if (!m_bParamInit) return;

    QMutexLocker locker(&m_mutex);
    while (!m_logQueue.isEmpty())
    {
        QString message = m_logQueue.dequeue();
        if (!message.isEmpty())
        {
            writeToFile(message);
        }
    }
}
