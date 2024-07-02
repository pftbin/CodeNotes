#include "TCPClient.h"

#define UTF16BE_ENABLE  (1)             //当前是否收发UTF-16BE编码的消息
#define RCVBUF_SIZE		(1024*20)       //20K
#define MAX_MSGSIZE		(1024*1024*10)  //10M

#include <QByteArray>
#include <QtCore/qendian.h>
static QString utf16beToUtf8(QByteArray &messageData)
{
    QString strMessage;
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
    {
        int dataSize = messageData.size() / 2;
        char16_t* msg16 = (char16_t*)messageData.data();
        for (int i = 0; i < dataSize; i++)
        {
            msg16[i] = qFromBigEndian<int16_t>(msg16[i]);
        }
        strMessage = QString::fromUtf16(msg16, dataSize);
    }
    else
    {
        strMessage = QString::fromUtf16((char16_t*)(messageData.data()), messageData.size() / 2);
    }

    return strMessage;
}
static QByteArray utf8ToUtf16be(QString &messageData)
{
    QByteArray replayData;
    std::u16string str16 = messageData.toStdU16String();
    int dataSize = str16.size();
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
    {
        char16_t* data16 = str16.data();
        for (int i = 0; i < dataSize; i++)
        {
            data16[i] = qToBigEndian<int16_t>(data16[i]);
        }
        replayData = QByteArray((char*)data16, dataSize * 2);
    }
    else
    {
        replayData = QByteArray((char*)str16.data(), dataSize * 2);
    }

    return replayData;
}


TCPClient::TCPClient(QObject *parent)
    : QObject(parent)
{
    m_nCodeType = Type_UTF8;
    m_pTCPHandler = NULL;
    m_pMsgBlockStrategy = NULL;

    m_rcvBuf = new char[RCVBUF_SIZE];
    m_rcvBufLen = RCVBUF_SIZE;
    m_rcvedLen = 0;

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &TCPClient::handleRecvMessage);
    connect(m_socket, &QTcpSocket::connected, this, &TCPClient::handleConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TCPClient::handleDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TCPClient::handleError);
}

TCPClient::~TCPClient()
{
    delete m_socket;
    m_socket = NULL;

    delete [] m_rcvBuf;
    m_rcvBufLen = 0;
    m_rcvedLen = 0;
}

void TCPClient::setHandler(ITCPHandler* pTCPHandler, IMsgBlockStrategy* pMsgBlockStrategy)
{
    m_pTCPHandler = pTCPHandler;
    m_pMsgBlockStrategy = pMsgBlockStrategy;
}
void TCPClient::connectToServer(const QString &ip, quint16 port)
{
    QHostAddress address(ip);
    m_socket->connectToHost(address, port);
}
bool TCPClient::isConnected()
{
    return (m_socket && m_socket->state()==QAbstractSocket::ConnectedState);
}
bool TCPClient::sendMessage(QString &message)
{
    bool bSend = false;
    QByteArray arrayData_final;
    QByteArray arrayData = message.toUtf8();
    if (m_nCodeType == Type_UTF8)
    {
        arrayData_final = arrayData;
    }
    else if (m_nCodeType == Type_UTF16)
    {
        arrayData_final = utf8ToUtf16be(message);
    }

    if (m_socket)
    {
        if (m_socket->state() == QAbstractSocket::ConnectedState)
        {
            if (m_socket->write(arrayData_final) == arrayData_final.size())
            {
                m_socket->flush();
                bSend = true;
            }
        }
    }

    return bSend;
}
bool TCPClient::sendMessage(QByteArray &message)
{
    bool bSend = false;
    QByteArray arrayData_final = message;
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->write(arrayData_final);
        m_socket->flush();
        bSend = true;
    }

    return bSend;
}

void TCPClient::handleRecvMessage()
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());

    if (pClientSocket && pClientSocket->bytesAvailable() > 0)
    {
        int nOldRcved = m_rcvedLen;
        if(m_rcvedLen > MAX_MSGSIZE) return;
        if(m_rcvedLen >= m_rcvBufLen) //更新总数据Buffer大小
        {
            char* buf = new char[m_rcvedLen+ RCVBUF_SIZE];
            memcpy(buf, m_rcvBuf, m_rcvedLen);
            delete [] m_rcvBuf;
            m_rcvBuf = buf;
            m_rcvBufLen = m_rcvedLen + RCVBUF_SIZE;
        }

        QByteArray arrayData_final;
        QByteArray arrayData = pClientSocket->readAll();
        if (m_nCodeType == Type_UTF8)
        {
            arrayData_final = arrayData;
        }
        else if (m_nCodeType == Type_UTF16)
        {
            QString string_utf8 = utf16beToUtf8(arrayData);
            arrayData_final = string_utf8.toUtf8();
        }

        char* pBuff = arrayData_final.data(); int nBuffLen = arrayData_final.size();
        memcpy(m_rcvBuf+m_rcvedLen, pBuff, nBuffLen);//将本次接收的数据追加到总数据Buffer
        m_rcvedLen += nBuffLen;

        if(nOldRcved < m_rcvedLen)
        {
            if (m_pMsgBlockStrategy)
            {
                //检查本次消息是否含有消息头,如有则追加后的数据向前移覆盖消息头
                //收到完整消息后会覆盖到Buffer起始位置,m_rcvedLen清0
                int nMsgLen = 0;
                do {
                    nMsgLen = m_pMsgBlockStrategy->GetMsgBlock(m_rcvBuf, m_rcvedLen);
                    if(nMsgLen > 0)
                    {
                        int nMin = fmin(nMsgLen, m_rcvedLen);
                        if (m_pTCPHandler)
                            m_pTCPHandler->HandleInput(pClientSocket, NULL, m_rcvBuf, nMin);

                        m_rcvedLen -= nMin;
                        memmove(m_rcvBuf, m_rcvBuf+nMin, m_rcvedLen);
                    }
                }while(nMsgLen > 0 && m_rcvedLen > 0);
            }
            else
            {
                if (m_pTCPHandler)
                    m_pTCPHandler->HandleInput(pClientSocket, NULL, m_rcvBuf, m_rcvedLen);

                m_rcvedLen = 0;
            }
        }

        QString message = QString::fromUtf8(arrayData_final);
        emit messageReceived(message);
    }
}
void TCPClient::handleConnected()
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());
    if (m_pTCPHandler)
        m_pTCPHandler->HandleConnect(pClientSocket, NULL, true);
}
void TCPClient::handleDisconnected()
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());
    if (pClientSocket)
        pClientSocket->close();

    if (m_pTCPHandler)
        m_pTCPHandler->HandleClose(pClientSocket, NULL);

    emit reconectNotify();
}
void TCPClient::handleError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());
    if (pClientSocket)
        pClientSocket->close();

    if (m_pTCPHandler)
        m_pTCPHandler->HandleClose(pClientSocket, NULL);

    qDebug() << "Socket error:" << m_socket->errorString();
}
