#ifndef QLOGGER_H
#define QLOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <QThread>

class QLogger : public QObject
{
    Q_OBJECT

public:
    explicit QLogger(QObject *parent = nullptr);
    virtual ~QLogger();

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
extern QLogger logger;

#define IMPLEMENT_LOGER(logger) \
QLogger logger(nullptr);

#define INIT_LOGER(logger,filename,folder) \
logger.SetLogParam(filename,folder);

#define WRITE_LOG(logger,message,sync) \
logger.writeLog(message,sync);


#endif
