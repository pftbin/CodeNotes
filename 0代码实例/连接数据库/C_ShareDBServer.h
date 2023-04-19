#pragma once
#include "..\ShareDBClient\IShareDBc.h"

class C_ShareDBServer
{
public:
	C_ShareDBServer(void);
	virtual~C_ShareDBServer(void);

	BOOL InitServer();

public:
	BOOL GetCGPlayListInfo(int nID, CString& strPlayListInfo);

protected:
	IShareDBc*	m_pShareDB;
	HMODULE		m_hModuleShare;

	BOOL InitShareDBClient();
	BOOL ExitShareDBClient();


};
