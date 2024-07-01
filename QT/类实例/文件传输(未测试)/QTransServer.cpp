#include "QTransServer.h"
#include <QSettings>
#include <QCoreApplication>

QTransServer::QTransServer(QObject *parent)
    : QObject{parent}
{
    m_pTcpServer = new QTcpServer(this);
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &QTransServer::newConnection);

    QString currentDir = QCoreApplication::applicationDirPath();
    QString strIniFilePath = QString("%1/Config.ini").arg(currentDir);
    QSettings settings(strIniFilePath, QSettings::IniFormat);

    m_nListenPort = settings.value("TCP/Port").toInt();
    if (m_nListenPort >= 0)
        m_nListenPort = 1088;

}

bool QTransServer::startServer()
{
    if (!m_pTcpServer->listen(QHostAddress::Any, m_nListenPort))
    {
        return false;
    }

    return true;
}

void QTransServer::newConnection()
{
    m_pTcpSocket = m_pTcpServer->nextPendingConnection();
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &QTransServer::receiveFilePart);
}
void QTransServer::receiveFilePart()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_15);

    if (m_nFileSize == 0)
    {
        if (m_pTcpSocket->bytesAvailable() < sizeof(qint64))
            return;
        in >> m_nFileSize;

        m_FileObject.setFileName(m_strFilePath);
        if (!m_FileObject.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error opening file!";
            return;
        }
        m_nRecvSize = 0;
    }

    while (m_pTcpSocket->bytesAvailable() > 0)
    {
        QByteArray data = m_pTcpSocket->readAll();
        m_nRecvSize += data.size();
        m_FileObject.write(data);
        if (m_nRecvSize == m_nFileSize)
        {
            m_FileObject.close();
            qDebug() << "File received!";
            m_nFileSize = 0;
            m_nRecvSize = 0;
        }
    }
}
