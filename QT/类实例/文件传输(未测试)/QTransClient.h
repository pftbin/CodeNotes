#ifndef QTRANSCLIENT_H
#define QTRANSCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class QTransClient : public QObject
{
    Q_OBJECT
public:
    explicit QTransClient(QObject *parent = nullptr);

public:
    bool connectToServer();
    bool sendFileData(QString strFilePath);

private:
    QTcpSocket *m_pTcpSocket;
    QString     m_strServerIP;
    qint16      m_nServerPort;


    QFile       m_FileObject;
    qint64      m_nFileSize;
    qint64      m_nSendSize;

signals:

};

#endif // QTRANSCLIENT_H
