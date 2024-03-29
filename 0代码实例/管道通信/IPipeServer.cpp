#include "StdAfx.h"
#include "IPipeServer.h"

DWORD WINAPI THREAD_SERVER(LPVOID lp)
{
	IPipeServer* pServer = (IPipeServer*)lp;
	return pServer->PipeServerWork();
}

IPipeServer::IPipeServer(void)
{
	m_strPipeName	= _T("\\\\.\\Pipe\\MyPipeTest");//管道名，以此区分管道，相同即可通信
	m_nPipeClass    = 0;

	m_hPipe			= NULL;
	m_nStackSize	= 1024;
	m_hThread = CreateThread(NULL, m_nStackSize, THREAD_SERVER, this, 0, NULL);
}

IPipeServer::~IPipeServer(void)
{
	Release();
}

DWORD IPipeServer::PipeServerWork()
{
	::CoInitialize (NULL);
	CString strInfo;
	if(InitPipeServer())
	{
		strInfo.Format (_T("连接管道[%s]成功!"), m_strPipeName);
		BOOL bRet = FALSE;
		while(TRUE)
		{
			bRet = ConnectNamedPipe(m_hPipe, NULL);
			if (!bRet) // 还没有断开，
			{
				if (ERROR_PIPE_CONNECTED == GetLastError())  // 客户还在连接中
				{
					strInfo.Format(_T("管道[%s]连接正常."), m_strPipeName);
				}
				if (ERROR_NO_DATA == GetLastError())         // 客户已经关闭
				{
					DisconnectNamedPipe(m_hPipe);
					strInfo.Format(_T("管道[%s]客户已经关闭，等待客户."),m_strPipeName);
					continue;
				}
			}
			else
			{
				strInfo.Format(_T("管道[%s]有客户接入."),m_strPipeName);
			}

			CString strReceiveMessage,strResut;
			DWORD	dwReadCount = 0;
			strInfo.Format(_T("管道[%s]开始读取数据."),m_strPipeName);
			if(ReadPipeData(strReceiveMessage, dwReadCount))
			{
				//此处调用虚函数
				bRet = DealWithCMD(strReceiveMessage,strResut);
				
				//CString strMsg;strMsg.Format(_T("服务端读取到数据: %s"),strReceiveMessage);
				//AfxMessageBox(strMsg);
			}
			else
			{
				strInfo.Format(_T("服务端读取数据失败"));
				//AfxMessageBox(strInfo);
			}			
		}
	}
	else
	{
		strInfo.Format (_T("连接管道[%s]失败!"), m_strPipeName);
	}
	::CoUninitialize();

	return TRUE;
}

void IPipeServer::Release()
{
	CString strInfo;
	if(NULL != m_hThread)
	{
		strInfo.Format(_T("正在析构通道【%s】线程..."),m_strPipeName);
		CloseHandle (m_hThread);
		m_hThread = NULL;
		Sleep(500);
	}
	if(NULL != m_hPipe)
	{
		strInfo.Format(_T("正在断开通道【%s】..."),m_strPipeName);
		DisconnectNamedPipe(m_hPipe);
		CloseHandle (m_hPipe);
		m_hPipe = NULL;
		Sleep(500);
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL IPipeServer::InitPipeServer()
{
	BOOL bRet = FALSE;
	//m_hPipe = CreateNamedPipe(m_strPipeName,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,1,m_nStackSize,m_nStackSize,1000,NULL);
	m_hPipe = CreateNamedPipe(m_strPipeName, PIPE_ACCESS_INBOUND,PIPE_TYPE_BYTE | PIPE_WAIT, 5, m_nStackSize, m_nStackSize, 500, NULL);
	if(NULL != m_hPipe)
	{
		bRet = ConnectNamedPipe(m_hPipe,NULL);
	}
	return bRet;
}

BOOL IPipeServer::WritePipeData(CString strMessage, DWORD& dwWriteCount)
{
	if(NULL == m_hPipe || strMessage.IsEmpty())
	{
		return FALSE;
	}
	int nLength = strMessage.GetLength() * sizeof(TCHAR);
	return WriteFile(m_hPipe, strMessage, nLength, &dwWriteCount, NULL);
}

BOOL IPipeServer::ReadPipeData(CString& strMessage, DWORD& dwReadCount)
{
	BOOL bRet = TRUE;
	int nError = 0;
	CString strLog;
	OVERLAPPED   overlappedInfo;
	memset(&overlappedInfo, 0, sizeof(OVERLAPPED));

	overlappedInfo.Offset = 0;
	overlappedInfo.OffsetHigh = 0;

	char* buffer = new char[BUFLENGTH];
	memset (buffer,0,BUFLENGTH);

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, 0);
	overlappedInfo.hEvent = hEvent;
	bRet = ReadFile(m_hPipe ,buffer, BUFLENGTH * sizeof(char), &dwReadCount, NULL);
	nError = GetLastError();
	if (!bRet && (ERROR_IO_PENDING == nError)) 
	{
		do 
		{
			strLog.Format(_T("暂时没有数据[%s]"),m_strPipeName);
		} 
		while (WAIT_TIMEOUT == WaitForSingleObject(overlappedInfo.hEvent, 5000));
	}
	if(bRet)
	{
		int nSize = (dwReadCount + 2) / sizeof(TCHAR);
		TCHAR* tc = new TCHAR[nSize];
		memset (tc, 0, nSize);
		memcpy (tc,buffer,dwReadCount);
		tc[nSize - 1]  = _T('\0');
		strMessage = tc;
		delete []tc;

		CString strReadData;
		strReadData.Format(_T("读取管道[%s]数据:%s"), m_strPipeName, strMessage);
	}

	delete []buffer;
	CloseHandle (hEvent);
	return bRet;
}



