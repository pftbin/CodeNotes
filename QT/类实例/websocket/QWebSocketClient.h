#ifndef QWEBSOCKETCLIENT_H
#define QWEBSOCKETCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QWebSocketProtocol>
#include <QDebug>
#include <QTimer>

class QWebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit QWebSocketClient(const QUrl &url, QObject *parent = nullptr);
    ~QWebSocketClient();

    bool connectToServer();
    void sendTextMessage(const QString &message);

signals:
    void textMessageReceived(const QString &message);

private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void onConnectionError(QAbstractSocket::SocketError error);

private:
    QWebSocket      m_ws;
    QUrl            m_url;
    QTimer          m_reconnectTimer;
    int             m_reconnectCount;
    QList<QString>  m_pendingMessages;

};

#endif // QWEBSOCKETCLIENT_H
