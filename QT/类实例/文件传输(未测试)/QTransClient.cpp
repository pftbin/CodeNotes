#include "QTransClient.h"
#include <QSettings>
#include <QCoreApplication>

QTransClient::QTransClient(QObject *parent)
    : QObject{parent}
{
    m_pTcpSocket = new QTcpSocket(this);

    QString currentDir = QCoreApplication::applicationDirPath();
    QString strIniFilePath = QString("%1/Config.ini").arg(currentDir);
    QSettings settings(strIniFilePath, QSettings::IniFormat);

    m_strServerIP = settings.value("TCP/IP").toString();
    m_nServerPort = settings.value("TCP/Port").toInt();
    if (m_nServerPort >= 0)
        m_nServerPort = 1088;

}


bool QTransClient::connectToServer()
{
    if (m_strServerIP.isEmpty())
        return false;

    m_pTcpSocket->connectToHost(m_strServerIP, m_nServerPort);
    if (!m_pTcpSocket->waitForConnected())
    {
        qDebug() << "Connecting to server failed!";
    }
}

bool QTransClient::sendFileData(QString strFilePath)
{
    if (m_nFileSize == 0)
    {
        m_FileObject.setFileName(strFilePath);
        if (!m_FileObject.open(QIODevice::ReadOnly))
        {
            qDebug() << "Opening file failed!";
            return false;
        }

        m_nSendSize = 0;
        m_nFileSize = m_FileObject.size();
        QDataStream out(m_pTcpSocket);
        out.setVersion(QDataStream::Qt_5_15);
        out << m_nFileSize;
    }

    while (m_nSendSize < m_nFileSize)
    {
        QByteArray data = m_FileObject.read(8192);
        qint64 bytesWritten = m_pTcpSocket->write(data);
        if (bytesWritten == -1)
        {
            qDebug() << "Writing data to socket failed!";
            return false;
        }
        m_nSendSize += bytesWritten;
    }

    if (m_nSendSize == m_nFileSize)
    {
        m_FileObject.close();
        qDebug() << "File send success!";
    }
}
