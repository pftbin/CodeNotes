#include "StdAfx.h"
#include "IPipeClient.h"
#include <ShellAPI.h>

DWORD WINAPI THREAD_CLIENT(LPVOID lp)
{
	IPipeClient* pClient = (IPipeClient*)lp;
	return pClient->PipeClientWork();
}

IPipeClient::IPipeClient(void)
{
	m_strPipeName	= _T("\\\\.\\Pipe\\MyPipeTest");//�ܵ������Դ����ֹܵ�����ͬ����ͨ��
	m_nPipeClass    = 0;

	m_hPipe			= NULL;
	m_nStackSize	= 1024;
	m_hThread = CreateThread(NULL, m_nStackSize, THREAD_CLIENT, this, 0, NULL);

	InitializeCriticalSection(&m_csection);
}

IPipeClient::~IPipeClient(void)
{
	Release();
}

DWORD IPipeClient::PipeClientWork()
{
	CString strLog;
	while(TRUE)
	{
		PIPECMD PipeCmd;
		if(GetPipeCmd (PipeCmd))
		{
			if(OpenPipe())
			{
				CString strSendMessage = PipeCmd.strReceive;
				DWORD dwWriteCount = 0;
				if(WritePipeData (strSendMessage, dwWriteCount))
				{
					Sleep(50);
					ClosePipe();
					strLog.Format (_T("д��ܵ�[%s]����[%s] д�볤��[%d]�ֽ�."),m_strPipeName ,strSendMessage, dwWriteCount);
				}
				else
				{
					ClosePipe();
					strLog.Format (_T("д��ܵ�[%s]����[%s]ʧ��!."),m_strPipeName ,strSendMessage);
				}
			}
			else
			{
				strLog.Format (_T("�򿪹ܵ�[%s]ʧ��!."),m_strPipeName);
				Sleep (1000);
			}
		}
		Sleep (40);
	}

	return TRUE;
}

void IPipeClient::Release()
{
	if(m_hThread && WaitForSingleObject(m_hThread,2000) != WAIT_OBJECT_0)
	{
		TerminateThread(m_hThread,0);
		CloseHandle (m_hThread);
		m_hThread	= NULL;
		CString strLog;
		strLog.Format (_T("�ܵ��߳�[%s]�ȴ�2��ǿ���˳��߳�"),m_strPipeName);
	}

	if(NULL != m_hThread)
	{
		CloseHandle (m_hThread);
		m_hThread	= NULL;
	}

	if(NULL != m_hPipe)
	{
		CloseHandle (m_hPipe);
		m_hPipe		= NULL;
	}

	::DeleteCriticalSection (&m_csection);
}

void IPipeClient::AddCmd(const PIPECMD& PipeCmd)
{
	EnterCriticalSection (&m_csection);
	CString strInfo;
	PIPECMD	Cmd = PipeCmd;
	m_PipeCmds.push_back (Cmd);
	::LeaveCriticalSection(&m_csection);
}

//////////////////////////////////////////////////////////////////////////
BOOL IPipeClient::GetPipeCmd(PIPECMD& PipeCmd)
{
	BOOL bRet = FALSE;
	EnterCriticalSection (&m_csection);
	if(m_PipeCmds.size() > 0)
	{
		std::vector<PIPECMD>::iterator iter = m_PipeCmds.begin();
		PipeCmd = *iter;
		m_PipeCmds.erase (iter);
		bRet = TRUE;
	}
	::LeaveCriticalSection(&m_csection);
	return bRet;
}

BOOL IPipeClient::WritePipeData(CString strMessage, DWORD& dwWriteCount)
{
	if(NULL == m_hPipe || strMessage.IsEmpty())
	{
		return FALSE;
	}
	int nLength = strMessage.GetLength() * sizeof(TCHAR);
	return WriteFile (m_hPipe, strMessage, nLength, &dwWriteCount, NULL);
}

BOOL IPipeClient::OpenPipe()
{
	BOOL bRet = ::WaitNamedPipe(m_strPipeName, 3000);
	CString strLog;
	if (!bRet)
	{
		strLog.Format(_T("�ܵ�[%s]æ����û������"), m_strPipeName);
	}
	m_hPipe  = ::CreateFile(m_strPipeName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hPipe)
	{

		DWORD dwErr = GetLastError();
		strLog.Format(_T("�ܵ�[%s]����ʧ��!�������[%d]"), m_strPipeName, dwErr);
	}
	else
	{
		strLog.Format(_T("�ܵ�[%s]�����ɹ�!"), m_strPipeName);
		bRet = TRUE;
	}
	return bRet;
}

BOOL IPipeClient::ClosePipe()
{
	if(NULL != m_hPipe)
	{
		CloseHandle (m_hPipe);
		m_hPipe = NULL;
	}
	return TRUE;
}