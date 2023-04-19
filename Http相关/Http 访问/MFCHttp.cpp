#define  DF_POST_BUFFER_LENGTH		(1024)
#define  DF_GET_BUFFER_LENGTH		(10240)

class _autoRelease
{
public:
	_autoRelease(char *pdel):pbuffer(pdel)
	{
		pwbuffer = NULL;
	}
	_autoRelease(WCHAR *pdel):pwbuffer(pdel)
	{
		pbuffer = NULL;
	}
	~_autoRelease()
	{
		if(pbuffer)
		{
			delete []pbuffer;
			pbuffer = NULL;
		}
		if(pwbuffer)
		{
			delete []pwbuffer;
			pwbuffer = NULL;
		}
	}
protected:
	char *pbuffer;
	WCHAR *pwbuffer;
};
#define  AUTODEL(pbuff,name)  _autoRelease (name)(pbuff)

void UTF8ToUnicode(char* pUTF8Src, WCHAR** ppUnicodeDst)
{

	int nUnicodeLen;        //转换后Unicode的长度

	//获得转换后的长度，并分配内存
	nUnicodeLen = MultiByteToWideChar(CP_UTF8,
		0,
		pUTF8Src,
		-1,
		NULL,
		0);

	nUnicodeLen += 1;
	*ppUnicodeDst = new WCHAR[nUnicodeLen];

	//转为Unicode
	MultiByteToWideChar(CP_UTF8,
		0,
		pUTF8Src,
		-1,
		*ppUnicodeDst,
		nUnicodeLen);
}

void UnicodeToUTF8(const WCHAR *pUnicodeSrc, char** ppUTF8Dst)
{
	/* get output buffer length */
	int		iUTF8Len(0);
	// wide char to multi char
	iUTF8Len = WideCharToMultiByte(CP_UTF8,
		0,
		pUnicodeSrc,
		-1,
		NULL,
		0,
		NULL,
		NULL );

	/* convert unicode to UTF8 */
	iUTF8Len += 1;
	*ppUTF8Dst = new char[iUTF8Len];
	memset(*ppUTF8Dst, 0, iUTF8Len);

	iUTF8Len = WideCharToMultiByte(CP_UTF8,
		0,
		pUnicodeSrc,
		-1,
		*ppUTF8Dst,
		iUTF8Len,
		NULL,
		NULL );

}

//POST 请求
BOOL Http_POST(CString strFullURLPath, CString strHeaders, CString strBody, std::string& sReceive)
{
	BOOL bResult = FALSE;

	//解析Url,例如：https://www.cnblogs.com/Zender/p/7596730.html
	DWORD dwServiceType;			// 协议类型	例如：http/https/...
	CString strHost;				// 主机地址	例如：www.cnblogs.com 或者 192.168.10.9
	CString strPath;				// 路径地址 例如：/Zender/p/7596730.html
	INTERNET_PORT wPort;			// 端口号	例如：http默认端口号是80,https的默认端口号是443
	if (!AfxParseURL(strFullURLPath, dwServiceType, strHost, strPath, wPort))
	{
		return bResult;//解析Url失败
	}

	CInternetSession session; // 创建会话 
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10*1000);// 超时设置为10s
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);		// 重试次数为2
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	if (dwServiceType == AFX_INET_SERVICE_HTTP)
	{
		pConnection = session.GetHttpConnection(strHost, wPort);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPath);
	}
	else if (dwServiceType == AFX_INET_SERVICE_HTTPS)
	{
		DWORD dwFlags = (INTERNET_FLAG_SECURE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD |INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);//SECURITY_FLAG_IGNORE_UNKNOWN_CA
		pConnection = session.GetHttpConnection(strHost, INTERNET_FLAG_SECURE, wPort);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPath, NULL, 1, NULL, NULL, dwFlags);
	}
	else
	{
		return bResult;//其他协议返回
	}

	try
	{
		//Header
		strHeaders += _T("Content-Type: application/json;charset=utf-8");

		//Body
		char* pBodyInfo(NULL);
		int nBodyLen = 0;
		if (!strBody.IsEmpty())
		{
			UnicodeToUTF8((LPCWSTR)strBody, &pBodyInfo);
			nBodyLen = strlen(pBodyInfo);
			AUTODEL(pBodyInfo, uniBody);
		}

		//Send
		bResult = pHttpFile->SendRequest(strHeaders, (LPVOID)pBodyInfo, nBodyLen);
		if (bResult==FALSE)
		{
			bResult = FALSE;
			CString strErrInfo;
			pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
			goto Failure_label;
		}

		//Status
		DWORD dwStatusCode = HTTP_STATUS_BAD_REQUEST;
		bResult = pHttpFile->QueryInfoStatusCode(dwStatusCode);
		if (bResult==FALSE || HTTP_STATUS_OK!=dwStatusCode)
		{
			bResult = FALSE;
			CString strErrInfo;
			pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
			goto Failure_label;
		}

		//Receive
		char buffer[DF_POST_BUFFER_LENGTH] = {0};
		for (UINT len = 0; (len = pHttpFile->Read(buffer, sizeof(buffer))) > 0; memset(buffer, 0, sizeof(buffer)))
		{
			std::string s(buffer, len);
			sReceive += s;
		}	
	}
	catch (CInternetException& e)
	{
		TCHAR szErr[512] = { 0 };
		e.GetErrorMessage(szErr, sizeof(szErr));

		bResult = FALSE;
		CString strErrInfo;
		pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
		goto Failure_label;
	}

Failure_label:
	if (pConnection)
	{
		pConnection->Close();
		delete pConnection;
		pConnection = NULL;
	}
	if (pHttpFile)
	{
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}
	session.Close();

	return bResult;
}

//GET 请求(下载文件)
BOOL Http_GET(CString strFullURLPath, CString strFullFilePath)
{
	if (!strFullFilePath.IsEmpty() && ::PathFileExists(strFullFilePath))
		return TRUE;

	BOOL bResult = FALSE;
	//解析Url,例如：https://www.cnblogs.com/Zender/p/7596730.html
	DWORD dwServiceType;			// 协议类型	例如：http/https/...
	CString strHost;				// 主机地址	例如：www.cnblogs.com 或者 192.168.10.9
	CString strPath;				// 路径地址 例如：/Zender/p/7596730.html
	INTERNET_PORT wPort;			// 端口号	例如：http默认端口号是80,https的默认端口号是443
	if (!AfxParseURL(strFullURLPath, dwServiceType, strHost, strPath, wPort))
	{
		return bResult;//解析Url失败
	}

	CInternetSession session;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 2000);	//重试等待2s
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);		//重试次数
	CHttpConnection* pConnection = NULL;
	CHttpFile* pHttpFile = NULL;
	if (dwServiceType==AFX_INET_SERVICE_HTTP)
	{
		pConnection = session.GetHttpConnection(strHost, wPort);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strPath);
	}
	else if (dwServiceType==AFX_INET_SERVICE_HTTPS)
	{
		DWORD dwFlags = (INTERNET_FLAG_SECURE | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD |INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);//SECURITY_FLAG_IGNORE_UNKNOWN_CA
		pConnection = session.GetHttpConnection(strHost, INTERNET_FLAG_SECURE, wPort);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strPath, NULL, 1, NULL, NULL, dwFlags);
	}
	else
	{
		return bResult;//其他协议返回
	}
	  
	try
	{
		//Send
		bResult = pHttpFile->SendRequest();
		if (bResult==FALSE)
		{
			bResult = FALSE;
			CString strErrInfo;
			pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
			goto Failure_label;
		}

		//Status
		DWORD dwStatusCode = HTTP_STATUS_BAD_REQUEST;
		bResult = pHttpFile->QueryInfoStatusCode(dwStatusCode);
		if (bResult==FALSE || HTTP_STATUS_OK!=dwStatusCode)
		{
			bResult = FALSE;
			CString strErrInfo;
			pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
			goto Failure_label;
		}

		//Receive
		HANDLE hFile = CreateFile(strFullFilePath, GENERIC_WRITE,FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //创建本地文件
		if(hFile == INVALID_HANDLE_VALUE)
		{
			bResult = FALSE;
			CString strErrInfo;
			pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
			goto Failure_label;
		}

		char szInfoBuffer[1024] = {0};  //返回消息
		DWORD dwFileSize = 0;			//文件长度
		DWORD dwInfoBufferSize = sizeof(szInfoBuffer);
		bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,(void*)szInfoBuffer,&dwInfoBufferSize,NULL);
		dwFileSize = atoi(szInfoBuffer);
		if (bResult && dwFileSize != 0)
		{
			char buffer[DF_GET_BUFFER_LENGTH] = {0};
			for (UINT len = 0; (len = pHttpFile->Read(buffer, sizeof(buffer))) > 0; memset(buffer, 0, sizeof(buffer)))
			{
				DWORD dwWrite = 0;
				WriteFile(hFile, buffer, len, &dwWrite, NULL);			//写到本地文件
			}
		}
		CloseHandle(hFile);
	}
	catch(CInternetException& e)
	{
		TCHAR szErr[512] = { 0 };
		e.GetErrorMessage(szErr, sizeof(szErr));

		bResult = FALSE;
		CString strErrInfo;
		pHttpFile->QueryInfo(HTTP_QUERY_RAW_HEADERS, strErrInfo);
		goto Failure_label;
	}

Failure_label:
	if (pConnection)
	{
		pConnection->Close();
		delete pConnection;
		pConnection = NULL;
	}
	if (pHttpFile)
	{
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}
	session.Close();

	return bResult;
}


//PUT未测试
#include <Wininet.h>  
                     
#pragma comment(lib, "Wininet.lib")  
BOOL HttpRequestPut(LPCTSTR pHomeUrl, LPCTSTR pPageUrl, LONG nPort, LPCTSTR pFile, CString *psRes, PBOOL pbExit)  
{  
    LPINTERNET_BUFFERS pBufferIn = new INTERNET_BUFFERS;  
    ZeroMemory(pBufferIn, sizeof(INTERNET_BUFFERS));  
    LPBYTE pBuf = new BYTE[1024];  
    HINTERNET hInternet = NULL;  
    HINTERNET hSession = NULL;  
    HINTERNET hRequest = NULL;  
    DWORD dwBytesRead;  
    DWORD dwBytesWritten;  
    HANDLE hFile = INVALID_HANDLE_VALUE;  
    CString sFormat(_T("Connection: Keep-Alive\r\n"));  
    sFormat += _T("Content-Type: application/octet-stream\r\n");  
    sFormat += _T("Content-Length: %u\r\n");  
    sFormat += _T("User-Agent:Test\r\n");  
    sFormat += _T("Host: %s:%u\r\n");  
    sFormat += _T("Accept: *.*,*/*\r\n");  
    sFormat += _T("\r\n");  
    CString sHeader(_T(""));  
    CString sRes(_T(""));  
                     
    do  
    {  
        hInternet = InternetOpen(_T("Mozilla/4.0 (compatible; Indy Library)"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);   
        if( NULL == hInternet )  
        {   
            sRes.Format(_T("Open link error. ErrCode=[%u]"), GetLastError());  
            break;  
        }  
                     
        hSession = InternetConnect(hInternet, pHomeUrl, (INTERNET_PORT)nPort, NULL, NULL,INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);  
        hRequest = HttpOpenRequest(hSession, _T("PUT"), pPageUrl,  NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE, 0);  
        if( FALSE == hRequest )  
        {  
            sRes.Format(_T("Open request handle error. ErrCode=[%u]"),  
                GetLastError());  
            break;  
        }  
                     
        hFile = CreateFile(pFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  
        if( hFile == INVALID_HANDLE_VALUE )  
        {  
            sRes.Format(_T("Open File error. ErrCode=[%u] File=[%s]"),  
                GetLastError(), pFile);  
            break;  
        }  
                     
        pBufferIn->dwStructSize = sizeof(INTERNET_BUFFERS);  
        pBufferIn->dwBufferTotal = GetFileSize(hFile, NULL);  
        sHeader.Format(sFormat, pBufferIn->dwBufferTotal, pHomeUrl, nPort);  
        pBufferIn->lpcszHeader = sHeader;  
        pBufferIn->dwHeadersLength = sHeader.GetLength();  
                     
        if( FALSE == HttpSendRequestEx(hRequest, pBufferIn, NULL, HSR_INITIATE, 0) )  
        {  
            sRes.Format(_T("Send request error."));  
            break;  
        }  
                     
        DWORD dwSendSize = 0;  
        while( dwSendSize < pBufferIn->dwBufferTotal )  
        {  
            if( (NULL!=pbExit) && (FALSE!=(*pbExit)) )  
            {  
                sRes.Format(_T("Stop upload because receive exit cmd."));  
                break;  
            }  
                     
            if( FALSE == ReadFile( hFile, pBuf, 1024, &dwBytesRead, NULL) )  
            {  
                sRes.Format(_T("Read File error. ErrCode=[%u] File=[%s]"),  
                    GetLastError(), pFile);  
                break;  
            }  
                     
            if( FALSE == InternetWriteFile(hRequest, pBuf, dwBytesRead, &dwBytesWritten) )  
            {// ERROR_INTERNET_CONNECTION_ABORTED  
                sRes.Format(_T("Upload File error. ErrCode=[%u] File=[%s]"), GetLastError(), pFile);  
                break;  
            }  
                     
            dwSendSize += dwBytesWritten;  
        }  
                     
        if( FALSE == HttpEndRequest(hRequest, NULL, 0, 0) )  
        {  
            sRes.Format(_T("End request error. ErrCode=[%u] File=[%s]"),  
                GetLastError(), pFile);  
        }  
       
    }while(FALSE);  
                     
    if( hFile != INVALID_HANDLE_VALUE )  
    {  
        CloseHandle(hFile);  
    }  
    if( hRequest )  
    {  
        InternetCloseHandle(hRequest);  
    }  
    if( hSession )  
    {  
        InternetCloseHandle(hSession);  
    }  
    if( hInternet )  
    {  
        InternetCloseHandle(hInternet);  
    }  
                     
    delete []pBuf;  
    delete pBufferIn;  
                     
    if( NULL != psRes )  
    {  
        *psRes = sRes;  
    }  
                     
    return sRes.IsEmpty();  
}



