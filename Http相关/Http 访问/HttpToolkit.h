#pragma once
#include <curl/curl.h>

namespace http_toolkit
{
	struct DataBlock
	{
		DataBlock(int nBufferSize = 1024 * 1000);
		~DataBlock();

		BYTE* pBuff;
		int nBufSize;
		int nPos;
	};
	
	void UTF8ToUnicode(char* pUTF8Src, WCHAR** ppUnicodeDst);
	void UnicodeToUTF8(const WCHAR *pUnicodeSrc, char** ppUTF8Dst);
	void URLEncode(char* szIn, char** pOut);
	
	//GET
	BOOL SendAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData);
	
	//POST
	BOOL PostAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData);
	
	//DELETE
	BOOL DeleteAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData);
	
	//PUT
	BOOL PutAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData);
	
	//GET-from data
	BOOL DownloadFile(LPCSTR lpRequestUTF8Encoded, LPCTSTR lpStorePath, CString &strFileName, BOOL bReplace);
};

