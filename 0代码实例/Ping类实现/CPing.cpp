#include "stdafx.h"
#include "CPing.h"

CPing::CPing()
{
	bValid = FALSE;
	WSADATA wsaData;			// WSADATA
	int nRet;					// General use return code

	// Dynamically load the ICMP.DLL
	hndlIcmp = LoadLibrary(_T("ICMP.DLL"));
	if (hndlIcmp == NULL)
	{
		return;
	}
	// Retrieve ICMP function pointers
	pIcmpCreateFile  = (HANDLE (WINAPI *)(void))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpCreateFile");
	pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpCloseHandle");
	pIcmpSendEcho = (DWORD (WINAPI *)
		(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho");
	// Check all the function pointers
	if (pIcmpCreateFile == NULL		|| 
		pIcmpCloseHandle == NULL	||
		pIcmpSendEcho == NULL)
	{
		FreeLibrary((HMODULE)hndlIcmp);
		return;
	}

	// Init WinSock
	nRet = WSAStartup(0x0101, &wsaData );
    if (nRet)
    {
        WSACleanup();
		FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
    // Check WinSock version
    if (0x0101 != wsaData.wVersion)
    {
        WSACleanup();
		FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
	bValid = TRUE;
}

CPing::~CPing()
{
    WSACleanup();
	FreeLibrary((HMODULE)hndlIcmp);
}

int CPing::Ping(char* strHost)
{
	struct in_addr iaDest;		// Internet address structure
	DWORD *dwAddress;			// IP Address
	IPINFO ipInfo;				// IP Options structure
	ICMPECHO icmpEcho;			// ICMP Echo reply buffer
	HANDLE hndlFile;			// Handle for IcmpCreateFile()

	if (!bValid)
		return FALSE;

	// Copy the IP address
	struct in_addr addr;
	addr.S_un.S_addr = inet_addr(strHost);

	char c[4];
	c[0] = addr.S_un.S_un_b.s_b1;
	c[1] = addr.S_un.S_un_b.s_b2;
	c[2] = addr.S_un.S_un_b.s_b3;
	c[3] = addr.S_un.S_un_b.s_b4;

	dwAddress = (DWORD *)c;

	// Get an ICMP echo request handle        
	hndlFile = pIcmpCreateFile();

	// Set some reasonable default values
	ipInfo.Ttl = 255;
	ipInfo.Tos = 0;
	ipInfo.IPFlags = 0;
	ipInfo.OptSize = 0;
	ipInfo.Options = NULL;
	icmpEcho.Status = 0;
	// Reqest an ICMP echo
	pIcmpSendEcho(
		hndlFile,		// Handle from IcmpCreateFile()
		*dwAddress,		// Destination IP address
		NULL,			// Pointer to buffer to send
		0,				// Size of buffer in bytes
		&ipInfo,		// Request options
		&icmpEcho,		// Reply buffer
		sizeof(struct tagICMPECHO),
		500);			// Time to wait in milliseconds
	// Print the results
	iaDest.s_addr = icmpEcho.Source;
	if (icmpEcho.Status)
	{
		return FALSE;
	}

	// Close the echo request file handle
	pIcmpCloseHandle(hndlFile);
	return TRUE;
}
