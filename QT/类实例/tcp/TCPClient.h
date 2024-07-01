#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>

#include <QMutex>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>

#include "TCPDefine.h"

class TCPClient : public QObject
{
    Q_OBJECT

public:
    explicit TCPClient(QObject *parent = nullptr);
    ~TCPClient();

public:
    void setCodeType(int nCodeType) { m_nCodeType = nCodeType; }
    void setHandler(ITCPHandler* pTCPHandler, IMsgBlockStrategy* pMsgBlockStrategy = NULL);

    void connectToServer(const QString &ip, quint16 port);
    bool sendMessage(QString &message);
    bool sendMessage(QByteArray &message);

signals:
    void messageReceived(const QString &message);

private slots:
    void handleRecvMessage();
    void handleConnected();
    void handleDisconnected();
    void handleError(QAbstractSocket::SocketError socketError);

private:
    int                         m_nCodeType;
    QTcpSocket*                 m_socket;
    ITCPHandler*                m_pTCPHandler;
    IMsgBlockStrategy*          m_pMsgBlockStrategy;

    char*                       m_rcvBuf;
    int                         m_rcvBufLen;
    int                         m_rcvedLen;
};

#endif // TCPCLIENT_H
