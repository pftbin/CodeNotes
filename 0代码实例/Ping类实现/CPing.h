#ifndef CPING_H
#define CPING_H

typedef struct tagIPINFO
{
	u_char Ttl;				// Time To Live
	u_char Tos;				// Type Of Service
	u_char IPFlags;			// IP flags
	u_char OptSize;			// Size of options data
	u_char FAR *Options;	// Options data buffer
}IPINFO;

typedef IPINFO* PIPINFO;

typedef struct tagICMPECHO
{
	u_long Source;			// Source address
	u_long Status;			// IP status
	u_long RTTime;			// Round trip time in milliseconds
	u_short DataSize;		// Reply data size
	u_short Reserved;		// Unknown
	void FAR *pData;		// Reply data buffer
	IPINFO	ipInfo;			// Reply options
}ICMPECHO;

typedef ICMPECHO* PICMPECHO;

class CPing
{
public:
	CPing();
	~CPing();
	BOOL Ping(char* strHost);
private:
	// ICMP.DLL Export Function Pointers
	HANDLE (WINAPI *pIcmpCreateFile)(VOID);
	BOOL (WINAPI *pIcmpCloseHandle)(HANDLE);
	DWORD (WINAPI *pIcmpSendEcho)
		(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD);
	HANDLE hndlIcmp;			// LoadLibrary() handle to ICMP.DLL
	BOOL bValid; // if it doesn't construct properly, it won't be valid
};

#endif