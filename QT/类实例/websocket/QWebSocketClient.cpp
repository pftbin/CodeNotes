#include "QWebSocketClient.h"

QWebSocketClient::QWebSocketClient(const QUrl &url, QObject *parent)
    : QObject(parent)
    , m_url(url)
    , m_reconnectCount(0)
{
    QObject::connect(&m_ws, &QWebSocket::connected, this, &QWebSocketClient::onConnected);
    QObject::connect(&m_ws, &QWebSocket::textMessageReceived, this, &QWebSocketClient::onTextMessageReceived);
    QObject::connect(&m_ws, &QWebSocket::disconnected, this, &QWebSocketClient::onDisconnected);
    QObject::connect(&m_ws, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &QWebSocketClient::onConnectionError);


    m_reconnectTimer.setInterval(5000); // 5 seconds
    m_reconnectTimer.setSingleShot(true);
    QObject::connect(&m_reconnectTimer, &QTimer::timeout, this, &QWebSocketClient::connectToServer);
}

QWebSocketClient::~QWebSocketClient()
{
    m_ws.close();
}

bool QWebSocketClient::connectToServer()
{
    m_ws.open(m_url);
    return true;
}

void QWebSocketClient::sendTextMessage(const QString &message)
{
    if (m_ws.state() == QAbstractSocket::ConnectedState)
    {
        m_ws.sendTextMessage(message);
        m_ws.flush();
    }
    else
    {
        // 如果当前不在连接状态，则将消息缓存起来
        m_pendingMessages.append(message);
    }
}

void QWebSocketClient::onConnected()
{
    qDebug() << "WebSocket connected.";

    // 重发所有缓存的消息
    for (const QString &message : m_pendingMessages)
    {
        m_ws.sendTextMessage(message);
        m_ws.flush();
    }
    m_pendingMessages.clear();

    emit textMessageReceived("WebSocket connected.");

    // 重置重连计数器
    m_reconnectCount = 0;
}

void QWebSocketClient::onTextMessageReceived(const QString &message)
{
    qDebug() << "Received: " << message;
    emit textMessageReceived(message);
}

void QWebSocketClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected.";

    if (m_reconnectCount < 5)
    {
        // 如果重连次数小于 5，则启动重连计时器
        m_reconnectTimer.start();
        m_reconnectCount++;
    }
    else
    {
        // 如果重连次数超过 5，则输出错误信息
        qDebug() << "WebSocket reconnection failed.";
    }
}

void QWebSocketClient::onConnectionError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket connection error: " << error;

    if (error == QAbstractSocket::SocketError::RemoteHostClosedError)
    {
        // 如果连接被远程主机关闭，则启动重连计时器
        m_reconnectTimer.start();
    }
}
