#include "StdAfx.h"
#include "IPipeServer.h"

DWORD WINAPI THREAD_SERVER(LPVOID lp)
{
	IPipeServer* pServer = (IPipeServer*)lp;
	return pServer->PipeServerWork();
}

IPipeServer::IPipeServer(void)
{
	m_strPipeName	= _T("\\\\.\\Pipe\\MyPipeTest");//�ܵ������Դ����ֹܵ�����ͬ����ͨ��
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
		strInfo.Format (_T("���ӹܵ�[%s]�ɹ�!"), m_strPipeName);
		BOOL bRet = FALSE;
		while(TRUE)
		{
			bRet = ConnectNamedPipe(m_hPipe, NULL);
			if (!bRet) // ��û�жϿ���
			{
				if (ERROR_PIPE_CONNECTED == GetLastError())  // �ͻ�����������
				{
					strInfo.Format(_T("�ܵ�[%s]��������."), m_strPipeName);
				}
				if (ERROR_NO_DATA == GetLastError())         // �ͻ��Ѿ��ر�
				{
					DisconnectNamedPipe(m_hPipe);
					strInfo.Format(_T("�ܵ�[%s]�ͻ��Ѿ��رգ��ȴ��ͻ�."),m_strPipeName);
					continue;
				}
			}
			else
			{
				strInfo.Format(_T("�ܵ�[%s]�пͻ�����."),m_strPipeName);
			}

			CString strReceiveMessage,strResut;
			DWORD	dwReadCount = 0;
			strInfo.Format(_T("�ܵ�[%s]��ʼ��ȡ����."),m_strPipeName);
			if(ReadPipeData(strReceiveMessage, dwReadCount))
			{
				//�˴������麯��
				bRet = DealWithCMD(strReceiveMessage,strResut);
				
				//CString strMsg;strMsg.Format(_T("����˶�ȡ������: %s"),strReceiveMessage);
				//AfxMessageBox(strMsg);
			}
			else
			{
				strInfo.Format(_T("����˶�ȡ����ʧ��"));
				//AfxMessageBox(strInfo);
			}			
		}
	}
	else
	{
		strInfo.Format (_T("���ӹܵ�[%s]ʧ��!"), m_strPipeName);
	}
	::CoUninitialize();

	return TRUE;
}

void IPipeServer::Release()
{
	CString strInfo;
	if(NULL != m_hThread)
	{
		strInfo.Format(_T("��������ͨ����%s���߳�..."),m_strPipeName);
		CloseHandle (m_hThread);
		m_hThread = NULL;
		Sleep(500);
	}
	if(NULL != m_hPipe)
	{
		strInfo.Format(_T("���ڶϿ�ͨ����%s��..."),m_strPipeName);
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
			strLog.Format(_T("��ʱû������[%s]"),m_strPipeName);
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
		strReadData.Format(_T("��ȡ�ܵ�[%s]����:%s"), m_strPipeName, strMessage);
	}

	delete []buffer;
	CloseHandle (hEvent);
	return bRet;
}



