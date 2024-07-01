#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QVector>

#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>

#include "TCPDefine.h"

class TCPServer : public QObject
{
    Q_OBJECT

public:
    explicit TCPServer(QObject *parent = nullptr);
    ~TCPServer();

public:
    void setCodeType(int nCodeType) { m_nCodeType = nCodeType; }
    void setHandler(ITCPHandler* pTCPHandler, IMsgBlockStrategy* pMsgBlockStrategy = NULL);
    bool start(quint16 port);
    bool start(QString ip, quint16 port);
    bool stop();

signals:


private slots:
    void handleNewConnection();
    void handleRecvMessage();
    void handleRemoveClient();

signals:
    void clientConnected(const QString &clientAddress);
    void clientDisconnected(const QString &clientAddress);

private:
    bool sendMessageToClient(QString &message, QTcpSocket *pClientSocket);
    bool sendMessageToClient(QByteArray &message, QTcpSocket *pClientSocket);

private:
    int                         m_nCodeType;
    QTcpServer*                 m_pTcpServer;
    QMutex                      m_lockClient;
    QVector<ClientNode*>        m_vecClient;
    ITCPHandler*                m_pTCPHandler;
    IMsgBlockStrategy*          m_pMsgBlockStrategy;

    char*                       m_rcvBuf;
    int                         m_rcvBufLen;
    int                         m_rcvedLen;

signals:

};

#endif // TCPSERVER_H
