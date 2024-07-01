#include "TCPServer.h"

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


TCPServer::TCPServer(QObject *parent)
    : QObject{parent}
{
    m_nCodeType = Type_UTF8;
    m_pTCPHandler = NULL;
    m_pMsgBlockStrategy = NULL;

    m_pTcpServer = NULL;
    m_vecClient.clear();

    m_rcvBuf = new char[RCVBUF_SIZE];
    m_rcvBufLen = RCVBUF_SIZE;
    m_rcvedLen = 0;
}
TCPServer::~TCPServer()
{
    stop();

    delete [] m_rcvBuf;
    m_rcvBufLen = 0;
    m_rcvedLen = 0;
}

void TCPServer::setHandler(ITCPHandler* pTCPHandler, IMsgBlockStrategy* pMsgBlockStrategy)
{
    m_pTCPHandler = pTCPHandler;
    m_pMsgBlockStrategy = pMsgBlockStrategy;
}
bool TCPServer::start(quint16 port)
{
    m_pTcpServer = new QTcpServer(this);
    if (!m_pTcpServer->listen(QHostAddress::Any, port))
        return false;

    connect(m_pTcpServer, &QTcpServer::newConnection, this, &TCPServer::handleNewConnection);

    return true;
}
bool TCPServer::start(QString ip, quint16 port)
{
    QHostAddress ipAddress;
    if (!ipAddress.setAddress(ip))
        return false;

    m_pTcpServer = new QTcpServer(this);
    if (!m_pTcpServer->listen(ipAddress, port))
        return false;

    connect(m_pTcpServer, &QTcpServer::newConnection, this, &TCPServer::handleNewConnection);

    return true;
}
bool TCPServer::stop()
{
    if (m_pTcpServer)
    {
        m_pTcpServer->close();
        delete m_pTcpServer;
        m_pTcpServer = nullptr;
    }
    m_vecClient.clear();

    return true;
}

void TCPServer::handleNewConnection()
{
    QTcpSocket* pClientSocket = m_pTcpServer->nextPendingConnection();
    ClientNode* pClientNode = new ClientNode;
    if (pClientNode && pClientSocket)
    {
        pClientNode->sock  = pClientSocket;
        pClientNode->srcip = "0.0.0.0";
        pClientNode->dstip = pClientSocket->peerAddress().toString();
    }

    m_lockClient.lock();
    bool bFind = false;
    QVector<ClientNode*>::iterator it = m_vecClient.begin();
    for (it; it != m_vecClient.end(); ++it)
    {
        if (pClientSocket && (*it)->srcip == pClientNode->srcip && (*it)->dstip == pClientNode->dstip)
        {
            bFind = true;
            (*it)->sock = pClientSocket;
        }
    }
    if (!bFind)
        m_vecClient.push_back(pClientNode);
    m_lockClient.unlock();

    connect(pClientSocket, &QTcpSocket::readyRead, this, &TCPServer::handleRecvMessage);
    connect(pClientSocket, &QTcpSocket::disconnected, this, &TCPServer::handleRemoveClient);

    if (m_pTCPHandler)
        m_pTCPHandler->HandleAccept(pClientSocket, pClientNode);

    emit clientConnected(pClientSocket->peerAddress().toString());
    qDebug() << "New client connected:" << pClientSocket->peerAddress().toString();
}
void TCPServer::handleRecvMessage()
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());
    ClientNode* pClientNode = NULL;
    m_lockClient.lock();
    QVector<ClientNode*>::iterator it = m_vecClient.begin();
    for (it; it != m_vecClient.end(); ++it)
    {
        if (pClientSocket && (*it)->sock == pClientSocket)
        {
            pClientNode = (*it);
            break;
        }
    }
    m_lockClient.unlock();


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
                            m_pTCPHandler->HandleInput(pClientSocket, pClientNode, m_rcvBuf, nMin);

                        m_rcvedLen -= nMin;
                        memmove(m_rcvBuf, m_rcvBuf+nMin, m_rcvedLen);
                    }
                }while(nMsgLen > 0 && m_rcvedLen > 0);
            }
            else
            {
                if (m_pTCPHandler)
                    m_pTCPHandler->HandleInput(pClientSocket, pClientNode, m_rcvBuf, m_rcvedLen);

                m_rcvedLen = 0;
            }
        }
    }
}
void TCPServer::handleRemoveClient()
{
    QTcpSocket *pClientSocket = static_cast<QTcpSocket*>(sender());
    ClientNode* pClientNode = NULL;
    m_lockClient.lock();
    QVector<ClientNode*>::iterator it = m_vecClient.begin();
    for (it; it != m_vecClient.end(); ++it)
    {
        if (pClientSocket && (*it)->sock == pClientSocket)
        {
            pClientNode = (*it);
            break;
        }
    }
    m_lockClient.unlock();

    if (pClientSocket)
    {
        QString targetSrcIp = "0.0.0.0";
        QString targetDstIp = "0.0.0.0";
        if (pClientSocket)
            targetDstIp = pClientSocket->peerAddress().toString();

        m_lockClient.lock();
        QVector<ClientNode*>::iterator it = m_vecClient.begin();
        for (it; it != m_vecClient.end(); ++it)
        {
            if ((*it)->srcip == targetSrcIp && (*it)->dstip == targetDstIp)
            {
                m_vecClient.erase(it);
                break;
            }
        }
        m_lockClient.unlock();
        pClientSocket->deleteLater();

        if (m_pTCPHandler)
            m_pTCPHandler->HandleClose(pClientSocket, pClientNode);
    }

    emit clientDisconnected(pClientSocket->peerAddress().toString());
    qDebug() << "Client disconnected:" << pClientSocket->peerAddress().toString();
}

bool TCPServer::sendMessageToClient(QString &message, QTcpSocket *pClientSocket)
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

    if (pClientSocket && pClientSocket->state() == QAbstractSocket::ConnectedState)
    {
        pClientSocket->write(arrayData_final);
        pClientSocket->flush();
        bSend = true;
    }

    return bSend;
}
bool TCPServer::sendMessageToClient(QByteArray &message, QTcpSocket *pClientSocket)
{
    bool bSend = false;
    QByteArray arrayData_final = message;
    if (pClientSocket && pClientSocket->state() == QAbstractSocket::ConnectedState)
    {
        pClientSocket->write(arrayData_final);
        pClientSocket->flush();
        bSend = true;
    }

    return bSend;
}
