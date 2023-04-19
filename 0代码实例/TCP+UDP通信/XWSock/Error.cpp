#include "StdAfx.h"
#include ".\error.h"

#define BEGIN_MAJORERROR_MAP() \
	CError::MajorErrorNode CError::s_majorMap[] = {

#define ON_MAJORERROR(iMajor, szMsg) \
	{ iMajor, szMsg, L##szMsg },

#define END_MAJORERROR_MAP() \
	{ -1, "", L"" }};

#define BEGIN_MINORERROR_MAP() \
	CError::MinorErrorNode CError::s_minorMap[] = {

#define ON_MINORERROR(iMajor, iMinor, szMsg) \
	{ iMajor, iMinor, szMsg, L##szMsg },

#define END_MINORERROR_MAP() \
	{ -1, -1, "", L"" }};

BEGIN_MAJORERROR_MAP()
	ON_MAJORERROR(0, "Success")
	ON_MAJORERROR(1, "Network setup failure")
	ON_MAJORERROR(2, "Network unload failure")
	ON_MAJORERROR(3, "Network operation failure")
	ON_MAJORERROR(4, "System resource failure")
END_MAJORERROR_MAP()

BEGIN_MINORERROR_MAP()
	ON_MINORERROR(1, 1, "UDP socket already created")
	ON_MINORERROR(1, 2, "unable to create UDP socket")
	ON_MINORERROR(1, 3, "unable to bind UDP socket")
	ON_MINORERROR(1, 4, "TCP socket already created")
	ON_MINORERROR(1, 5, "unable to create TCP socket")
	ON_MINORERROR(1, 6, "unable to bind TCP socket")
	ON_MINORERROR(3, 1, "unable to configure UDP socket")
	ON_MINORERROR(3, 2, "UDP socket is closed when sending data")
	ON_MINORERROR(3, 3, "unable to send UDP data")
	ON_MINORERROR(3, 4, "Invalid UDP socket ID")
	ON_MINORERROR(3, 5, "unable to configure TCP socket")
	ON_MINORERROR(3, 6, "Cannot listen on TCP socket with current state")
	ON_MINORERROR(3, 7, "unable to listen on TCP socket")
	ON_MINORERROR(3, 8, "Cannot connect on TCP socket with current state")
	ON_MINORERROR(3, 9, "unable to connect on TCP socket")
	ON_MINORERROR(3, 10, "TCP socket is closed when sending data")
	ON_MINORERROR(3, 11, "unable to send TCP data")
	ON_MINORERROR(3, 12, "Invalid TCP socket ID")
	ON_MINORERROR(3, 13, "unable to accept on TCP socket")
	ON_MINORERROR(3, 14, "unable to select event on TCP socket")
	ON_MINORERROR(3, 15, "The socket is marked as nonblocking and the requested operation would block")
	ON_MINORERROR(3, 16, "send timeout")
	ON_MINORERROR(4, 1, "unable to create new threads")
	ON_MINORERROR(4, 2, "unable to allocate buffers")
END_MINORERROR_MAP()

CError::CError(void)
{
	m_dwTlsIdx = TlsAlloc();
}

CError::~CError(void)
{
	TlsFree(m_dwTlsIdx);
}

void CError::SetError(int iMajor, int iMinor, DWORD iErrno)
{
	ErrorInfo* pErr = (ErrorInfo*)TlsGetValue(m_dwTlsIdx);
	if(pErr == NULL)
	{
		pErr = new ErrorInfo;
		TlsSetValue(m_dwTlsIdx, pErr);
	}
	pErr->iMajor = iMajor;
	pErr->iMinor = iMinor;
	pErr->iErrno = iErrno;
}

DWORD CError::GetErrorCode(void) const
{
	ErrorInfo* pErr = (ErrorInfo*)TlsGetValue(m_dwTlsIdx);
	if(pErr == NULL) return 0;
	return pErr->iMajor*100000 + pErr->iMinor;
}

const char* CError::GetErrorMessageA()
{
	static const int nMajorCount = sizeof(s_majorMap)/sizeof(struct MajorErrorNode) - 1;
	static const int nMinorCount = sizeof(s_minorMap)/sizeof(struct MinorErrorNode) - 1;

	char* szMsg = "Success";
	ErrorInfo* pErr = (ErrorInfo*)TlsGetValue(m_dwTlsIdx);
	if(pErr == NULL)
	{
		return NULL;
	}

	szMsg = (char*)pErr->szMsg;
	int iMajor = pErr->iMajor;
	int iMinor = pErr->iMinor;
	int iErrno = pErr->iErrno;
	size_t nsize  = sizeof(pErr->szMsg);
	int i=0;
	for(; i<nMajorCount; i++)
	{
		if(iMajor == s_majorMap[i].iMajor)
		{
			strncpy(szMsg, s_majorMap[i].pMsgA,nsize-1);
			szMsg[nsize-1] = '\0';
			break;
		}
	}

	if(i >= nMajorCount)
	{
		strncpy(szMsg, "Unknown error",nsize);
	}
	else
	{
		for(int i=0; i<nMinorCount; i++)
		{
			if(iMajor == s_minorMap[i].iMajor && iMinor == s_minorMap[i].iMinor)
			{
				char *pdot = ": ";
				if(strlen(szMsg) + strlen(pdot) < nsize)
				{
					strcpy(szMsg + strlen(szMsg), pdot);
				}
				if(strlen(szMsg) + strlen(s_minorMap[i].pMsgA) < nsize)
				{
					strcpy(szMsg + strlen(szMsg), s_minorMap[i].pMsgA);
				}
				break;
			}
		}
	}

	// Adding "errno" information
	if (iErrno != -1)
	{
		char *pdot = ": ";
		if(strlen(szMsg) + strlen(pdot) < nsize)
		{
			strcpy(szMsg + strlen(szMsg), pdot);
		}
		LPVOID lpMsgBuf;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, iErrno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&lpMsgBuf, 0, NULL);
		strncpy(szMsg + strlen(szMsg), (char*)lpMsgBuf, 2048u - strlen(szMsg) - 2u);
		LocalFree(lpMsgBuf);
	}

	return szMsg;
}

const wchar_t* CError::GetErrorMessageW()
{
	static const int nMajorCount = sizeof(s_majorMap)/sizeof(struct MajorErrorNode) - 1;
	static const int nMinorCount = sizeof(s_minorMap)/sizeof(struct MinorErrorNode) - 1;

	wchar_t* szMsg = L"Success";
	ErrorInfo* pErr = (ErrorInfo*)TlsGetValue(m_dwTlsIdx);
	if(pErr == NULL)
	{
		return NULL;
	}

	szMsg = pErr->szMsg;
	int iMajor = pErr->iMajor;
	int iMinor = pErr->iMinor;
	int iErrno = pErr->iErrno;
	size_t nsize  = sizeof(pErr->szMsg)/2;
	int i=0;
	for(; i<nMajorCount; i++)
	{
		if(iMajor == s_majorMap[i].iMajor)
		{
			wcscpy(szMsg, s_majorMap[i].pMsgW);
			break;
		}
	}

	if(i >= nMajorCount)
	{
		wcscpy(szMsg, L"Unknown error");
	}
	else
	{
		for(int i=0; i<nMinorCount; i++)
		{
			if(iMajor == s_minorMap[i].iMajor && iMinor == s_minorMap[i].iMinor)
			{
				wchar_t *pdot = L": ";
				if(wcslen(szMsg) + wcslen(pdot) <nsize)
				{
					wcscpy(szMsg + wcslen(szMsg), pdot);
				}
				if(wcslen(szMsg) + wcslen(s_minorMap[i].pMsgW) <nsize)
				{
					wcscpy(szMsg + wcslen(szMsg), s_minorMap[i].pMsgW);
				}
				break;
			}
		}
	}

	// Adding "errno" information
	if (iErrno != -1)
	{
		wchar_t *pdot = L": ";
		if(wcslen(szMsg) + wcslen(pdot) <nsize)
		{
			wcscpy(szMsg + wcslen(szMsg), pdot);
		}
		LPVOID lpMsgBuf;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, iErrno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)&lpMsgBuf, 0, NULL);
		wcsncpy(szMsg + wcslen(szMsg), (wchar_t*)lpMsgBuf, 2048 - wcslen(szMsg) - 2*sizeof(wchar_t));
		LocalFree(lpMsgBuf);
	}

	return szMsg;
}