#include "StdAfx.h"
#include "StandardSocket.h"

CStandardSocket::CStandardSocket(void)
{
	m_nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	m_ConnectSocket = INVALID_SOCKET;
	m_bConnected = FALSE;
}

CStandardSocket::~CStandardSocket(void)
{
	WSACleanup();
}

BOOL CStandardSocket::ConnectServer(CString strServerIP, int nServerPort)
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	

	USES_CONVERSION;
	CString strPort;
	strPort.Format(L"%d", nServerPort);

	m_nResult = getaddrinfo(W2A(strServerIP), W2A(strPort), &hints, &result);

	if (m_nResult != 0)
	{
		m_bConnected = FALSE;
		return FALSE;
	}

	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) 
	{
		m_ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_ConnectSocket == INVALID_SOCKET) 
		{
			freeaddrinfo(result);
			return FALSE;
		}

		m_nResult = connect( m_ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (m_nResult == SOCKET_ERROR) 
		{
			closesocket(m_ConnectSocket);
			m_ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	
	freeaddrinfo(result);

	if (m_ConnectSocket == INVALID_SOCKET) 
	{
		m_bConnected = FALSE;
	}
	else
	{
		m_bConnected = TRUE;
	}

	return m_bConnected;
}

BOOL CStandardSocket::SendData(BYTE* data, int nDataLen)
{
	if (!m_bConnected)
	{
		return FALSE;
	}

	char* buffer = new char[nDataLen];
	memcpy(buffer, data, nDataLen);

	m_nResult = send(m_ConnectSocket, buffer, nDataLen, 0);

	delete[] buffer;

	if (m_nResult == SOCKET_ERROR) 
	{
		closesocket(m_ConnectSocket);
		return FALSE;
	}

	m_nResult = shutdown(m_ConnectSocket, SD_SEND);
	if (m_nResult == SOCKET_ERROR) 
	{
		return FALSE;	
	}

	closesocket(m_ConnectSocket);
	m_bConnected = FALSE;

	return TRUE;
}