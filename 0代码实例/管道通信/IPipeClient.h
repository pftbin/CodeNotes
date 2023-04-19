#pragma once
#include "I_PipeDefine.h"

//////////////////////////////////////////////////////////////////////////
class IPipeClient
{
public:
	IPipeClient(void);
	virtual ~IPipeClient(void);
	DWORD	PipeClientWork();
	void	Release();

	void	AddCmd(const PIPECMD& PipeCmd);//通过命令传递要发送的信息

protected:
	CString		m_strPipeName;
	HANDLE		m_hPipe;
	int			m_nStackSize;
	HANDLE		m_hThread;

	CRITICAL_SECTION		m_csection;
	std::vector<PIPECMD>	m_PipeCmds;
	BOOL GetPipeCmd(PIPECMD& PipeCmd);

	BOOL WritePipeData(CString strMessage, DWORD& dwWriteCount);
	BOOL OpenPipe();
	BOOL ClosePipe();	
};
