// TcpDataComm.h: interface for the CTcpDataComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPDATACOMM_H__58E94778_07DF_4658_B97C_7321A6474E8B__INCLUDED_)
#define AFX_TCPDATACOMM_H__58E94778_07DF_4658_B97C_7321A6474E8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "TransferGlobal.h"

#include "winsock2.h"
#include <atlconv.h>
#pragma comment(lib, "wsock32.lib")

class  ITSMsgReport ;

#define   MAX_RECSEND_LEN  (4*1024*1024)

class   CTcpDataComm  
{
public:
	CTcpDataComm();
	virtual ~CTcpDataComm();

public:
	BOOL Init(CString LocalIP);
	BOOL Unit();
	BOOL StartListen(int& iPort);
	BOOL Connect(CString RemoteIP,int iRemotPort);
	void Stop() ;
	BOOL SendData(char* pData,int& ilen );
	BOOL RecData(char*pData,int& ilen);
private:
	static void _Listen_CB(LPVOID) ;
	       void _Listen();
		   BOOL _SleepForTcpOk();

private:
	SOCKET   m_ListenSocket   ;   // 用于监听的Socket 
	SOCKET   m_DataCommSocket ;   // 用于数据传输的Socket 
	CString  m_sLocalIP       ;
	BOOL     m_bInit          ;
	HANDLE   m_hListenThread  ;
	BOOL     m_bListenRun     ;
	ITSMsgReport*    m_pMsgReport ;
	char*    m_pSRBuffer ;
	BOOL     m_bTcpOk ;
//	MsgInfo_S   m_sMsgInfo    ;
	DWORD    m_dwTimeOut      ;


};

#endif // !defined(AFX_TCPDATACOMM_H__58E94778_07DF_4658_B97C_7321A6474E8B__INCLUDED_)
