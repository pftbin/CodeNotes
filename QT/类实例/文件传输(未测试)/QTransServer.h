#ifndef QTRANSSERVER_H
#define QTRANSSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>

class QTransServer : public QObject
{
    Q_OBJECT
public:
    explicit QTransServer(QObject *parent = nullptr);

public:
    bool startServer();

public slots:
    void newConnection();
    void receiveFilePart();

private:
    QTcpServer *m_pTcpServer;
    QTcpSocket *m_pTcpSocket;
    qint16      m_nListenPort;

    QFile       m_FileObject;
    qint64      m_nFileSize;
    qint64      m_nRecvSize;
    QString     m_strFilePath;

signals:

};

#endif // QTRANSSERVER_H
