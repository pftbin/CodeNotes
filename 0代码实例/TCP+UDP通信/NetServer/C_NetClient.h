#pragma once

#include <map>
#include <vector>

#include "..\XWSock\XWSock.h"
#pragma comment(lib, "XWSock.lib")

using namespace std;
using namespace XWSock;

//////////////////////////////////////////////////////////////////////////
class C_NetClient:
	public IUDPHandler, 
	public ITCPHandler, 
	public IMsgBlockStrategy
{
public:
	C_NetClient(void);
	virtual~C_NetClient(void);

	WSADATA						m_wsData;
	//TCP
	void SetTCPHandler(void* pTCPHandler);
	BOOL SendTCPMessage(CString strServerIP,CString strServerPort,int nMsgType,CString strMessage);

	//UDP
	void SetUDPHandler(void* pUDPHandler);
	BOOL SendUDPMessage(CString strServerIP,CString strServerPort,int nMsgType,CString strMessage);

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
