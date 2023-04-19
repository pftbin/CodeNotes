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

	BOOL SendAndRecv(LPCSTR lpRequestUTF8Encoded, DataBlock &headerData, DataBlock &bodyData);
	BOOL SendAndRecvWithHeader(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount);
	BOOL DownloadFile(LPCSTR lpRequestUTF8Encoded,  LPCTSTR lpStorePath,CString &strFileName,BOOL bReplace = FALSE);
	BOOL PutAndRecvWithHeader(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount,LPCSTR lpData);
	BOOL PostAndRecv(LPCSTR lpRequestUTF8Encoded, DataBlock &headerData, DataBlock &bodyData,LPCSTR lpData);
	BOOL PostAndRecvWithHeader(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount,LPCSTR lpData);
	BOOL PostAndRecvWithHeaderNew(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount);

	BOOL DeleteAndRecvWithHeader(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount,LPCSTR lpData);

	void UTF8ToUnicode(char* pUTF8Src, WCHAR** ppUnicodeDst);
	void UnicodeToUTF8(const WCHAR *pUnicodeSrc, char** ppUTF8Dst);
	void URLEncode(char* szIn, char** pOut);
};

