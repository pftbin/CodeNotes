#include <afxinet.h>
#include <WinInet.h>
#include <string>
using namespace std;


namespace MFCHttp
{
	//
	BOOL Http_POST(CString strFullURLPath, CString strHeaders, CString strBody, std::string& sReceive);
	BOOL Http_GET(CString strFullURLPath, CString strFullFilePath);
	BOOL Http_PUT(LPCTSTR pHomeUrl, LPCTSTR pPageUrl, LONG nPort, LPCTSTR pFile, CString* psRes, PBOOL pbExit);
};


