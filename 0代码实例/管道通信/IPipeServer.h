#pragma once
#include "I_PipeDefine.h"

//////////////////////////////////////////////////////////////////////////
class IPipeServer
{
public:
	IPipeServer(void);
	virtual~IPipeServer(void);
	DWORD	PipeServerWork();
	void	Release();

protected:
	CString		m_strPipeName;

	HANDLE		m_hPipe;
	int			m_nStackSize;
	HANDLE		m_hThread;

	BOOL	InitPipeServer();
	BOOL	WritePipeData(CString strMessage, DWORD& dwWriteCount);
	BOOL	ReadPipeData(CString& strMessage, DWORD& dwReadCount);
};
