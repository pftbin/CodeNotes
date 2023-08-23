#pragma once
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#endif

#include <iostream>
#include <string>

typedef void (*pRecvCallback)(char* pDataBuff, int DataLen);
class TCPClient
{
public:
	TCPClient();
	~TCPClient();

    bool Connect(std::string serverIP, int serverPort);
    void Disconnect();
    bool SendTcpMsg(void* pHeader, int nHeaderLen, char* pBuff, int nBuffLen);
    void RecvTcpMsg();
    void SetRecvMsgCallback(pRecvCallback callback);

private:
    int                 m_sockfd;
    std::string         m_strIP;
    int                 m_nPort;
    struct sockaddr_in  m_serveraddr;
    pRecvCallback       m_pCallBackFun;
};

