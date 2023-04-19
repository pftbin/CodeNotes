#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

class CStandardSocket
{
public:
	CStandardSocket(void);

	~CStandardSocket(void);

public:
	BOOL ConnectServer(CString strServerIP, int nServerPort);

	BOOL SendData(BYTE* data, int nDataLen);

private:

	WSADATA wsaData;

	SOCKET m_ConnectSocket;

	int m_nResult;

	BOOL m_bConnected;
};
