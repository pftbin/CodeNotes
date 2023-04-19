#pragma once

#include <map>
#include <vector>

#include "..\XWSock\XWSock.h"
#pragma comment(lib, "XWSock.lib")

using namespace std;
using namespace XWSock;

//////////////////////////////////////////////////////////////////////////
struct ClientNode 
{
	TCPSOCKET	tcpClientSocket;//可提升为类对象，在发送消息前可进行连接检查和重连
	UDPSOCKET	udpClientSocket;
	u_long		dstip;
	u_long      srcip;
	u_short		port;
	CString     RecvMsg;
	CString     SendMsg;

	ClientNode()
	{
		tcpClientSocket = INVALID_SOCKET;
		udpClientSocket = INVALID_SOCKET;
		dstip = 0;
		srcip = 0;
		port = 0;
		RecvMsg = _T("");
		SendMsg = _T("");
	}
};

//////////////////////////////////////////////////////////////////////////
class C_NetServer:
	public IUDPHandler, 
	public ITCPHandler, 
	public IMsgBlockStrategy
{
public:
	C_NetServer(void);
	virtual ~C_NetServer(void);

	WSADATA						m_wsData;

	//TCP
	u_short						m_tcpSvrPort;
	TCPSOCKET					m_tcpSvrSocket;
	CRITICAL_SECTION			m_lockTcpClientArray;
	std::vector<ClientNode*>	m_tcpClientArray;
	
	BOOL InitTCPServer(CString strPort);
	void SetTCPHandler(void* pTCPHandler);
	BOOL Send2TCPClientMsg(void* sock, int nMsgType, CString strMessage);

	//UDP
	u_short						m_udpSvrPort;
	UDPSOCKET					m_udpSvrSocket;
	CRITICAL_SECTION			m_lockUdpClientArray;
	std::vector<ClientNode>		m_udpClientArray;

	BOOL InitUDPServer(CString strPort);
	void SetUDPHandler(void* pUDPHandler);
	BOOL Send2UDPClientMsg(void* sock, int nMsgType, CString strMessage);

protected:
	u_short Checksum(const u_short *buf, int size);
	BOOL IsValidPacket_i(const char* buf, int len);
	CString GetStrIP(u_long uip);

protected:
	CString		m_strLogInfo;
	void*		m_TCPHandler;//void 可改为接口
	void*		m_UDPHandler;//void 可改为接口

	//IMsgBlockStrategy
	virtual int  GetMsgBlock(const char* buf, int len);
	//IUDPHandler
	virtual void HandleInput(UDPSOCKET s, void* act, char* buf, int len, u_long src, u_short port);
	//ITCPHandler
	virtual void HandleAccept(TCPSOCKET s, void* act);
	virtual void HandleConnect(TCPSOCKET s, void* act, BOOL bConnected);
	virtual void HandleInput(TCPSOCKET s, void* act, char* buf, int len);
	virtual void HandleOutput(TCPSOCKET s, void* act);
	virtual void HandleClose(TCPSOCKET s, void* act);
};
