#pragma once
#include "StdAfx.h"
#define MAX_UDP_MSG_SIZE 512*1024
typedef void (*MessageProcessingCallBack)(const LPBYTE lpbMsg, DWORD dwSizeOfMsg, LPTSTR lpszSrcIP, int iSrcPort, LPVOID lpCaller);

class CCommunicationUDP
{
public:
	CCommunicationUDP(void);
	virtual ~CCommunicationUDP(void);

public:
	BOOL Initialize(LPTSTR lpszLocalIP = _T(""), int nPort = 0);
	BOOL Unintialize();
	BOOL SetCallBack(LPVOID lpCaller, MessageProcessingCallBack lpCallBackFunc);
	BOOL StartListen();
	BOOL StopListen();
	BOOL SendMessage(LPTSTR lpszDestIP, int iDestPort, const LPBYTE lpbMsg, SIZE_T sizeofmsg);

	static UINT AFX_CDECL RecvThreadProcCallBack(LPVOID lpParam);
	UINT RecvThreadProc();

private:
	MessageProcessingCallBack m_lpCallBackFunc;
	LPVOID m_lpCaller;
	CString m_strLocalIP;
	SOCKET m_ReceivingSocket;
	BYTE m_byRecvBuffer[MAX_UDP_MSG_SIZE];
	CWinThread* m_pRecvThread;
	BOOL m_bRecvThreadRun;
};
