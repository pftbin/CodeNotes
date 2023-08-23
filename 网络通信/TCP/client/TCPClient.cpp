#include "TCPClient.h"

#include "public.h"
static FileWriter loger_tcpclient("tcpclient.log");

//===========================================================================================================================================
TCPClient::TCPClient()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        _debug_to(loger_tcpclient, 0, ("failed to initialize winsock...\n"));
    }
#endif

    m_sockfd = -1;
    m_strIP = "0.0.0.0";
    m_nPort = 0;
    m_pCallBackFun = nullptr;
}

TCPClient::~TCPClient()
{
    Disconnect();

#ifdef _WIN32
    WSACleanup();
#endif
}

bool TCPClient::Connect(std::string serverIP, int serverPort)
{
    // 创建socket
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1)
    {
        _debug_to(loger_tcpclient, 0, ("failed to create socket...\n"));
        return false;
    }

    // 设置服务器地址
    m_serveraddr.sin_family = AF_INET;
    m_serveraddr.sin_port = htons(serverPort);
    m_serveraddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

    // 连接服务器
    if (connect(m_sockfd, (struct sockaddr*)&m_serveraddr, sizeof(m_serveraddr)) < 0)
    {
        _debug_to(loger_tcpclient, 0, ("addr: %s:%u ,connect failed\n"), inet_ntoa(m_serveraddr.sin_addr), ntohs(m_serveraddr.sin_port));
        return false;
    }
    
    _debug_to(loger_tcpclient, 0, ("connect socket[%d]...\n"), m_sockfd);
    return true;
}

void TCPClient::Disconnect()
{
#ifdef _WIN32
    closesocket(m_sockfd);
#else
    close(sock);
#endif

    _debug_to(loger_tcpclient, 0, ("close socket[%d]...\n"), m_sockfd);
    m_sockfd = -1; 
}

bool TCPClient::SendTcpMsg(void* pHeader, int nHeaderLen, char* pBuff, int nBuffLen)
{
    if (m_sockfd == -1 && !Connect(m_strIP, m_nPort))
    {
        _debug_to(loger_tcpclient, 0, ("socket is invalid, reconnect failed...\n"));
        return false;
    }

    //
    int DataLen = nBuffLen + nHeaderLen;
    char* pDataBuff = new char[DataLen];
    if (pHeader && nHeaderLen > 0)
    {
        memcpy(pDataBuff, pHeader, nHeaderLen);
        memcpy(pDataBuff + nHeaderLen, pBuff, nBuffLen);
    }
    else
    {
        memcpy(pDataBuff, pBuff, nBuffLen);
    }

    //
    if (send(m_sockfd, pDataBuff, DataLen, 0) < 0)
    {
        _debug_to(loger_tcpclient, 0, ("failed to send message[%s]...\n"), pBuff);
        return false;
    }

    _debug_to(loger_tcpclient, 0, ("send message[%s]\n"), pBuff);
    return true;
}

void TCPClient::RecvTcpMsg()
{
    char RecvBuff[1024] = { 0 };
    while (true)
    {
        memset(RecvBuff, 0, sizeof(RecvBuff));
        int RecvLen = recv(m_sockfd, RecvBuff, sizeof(RecvBuff), 0);
        if (RecvLen <= 0)
        {
            // 连接断开或出现错误
            break;
        }

        if (m_pCallBackFun)
            m_pCallBackFun(RecvBuff, RecvLen);

        _debug_to(loger_tcpclient, 0, ("recv message[%s]\n"), RecvBuff);
    }
}

void TCPClient::SetRecvMsgCallback(pRecvCallback callback)
{
    m_pCallBackFun = callback;
}

