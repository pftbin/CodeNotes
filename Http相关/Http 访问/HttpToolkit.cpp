#include "StdAfx.h"
#include "HttpToolkit.h"
#include <io.h>

/*
	使用实例：
	1.GET-POST等使用类似,示例是POST
	
	//URL
	CString strHOST = g_HOST;
	CString strPATH = g_PATHGetItemDataList;
	CString strDesURL; strDesURL.Format(_T("%s%s"),strHOST,strPATH);

	char* pUTF8Request(NULL);
	UnicodeToUTF8((LPCWSTR)strDesURL, &pUTF8Request);
	AUTODEL(pUTF8Request,uni1);
	char* pEncodedRequest(NULL);
	URLEncode(pUTF8Request, &pEncodedRequest);
	AUTODEL(pEncodedRequest,uni2);

	//Header
	char* pUTF8Header[1];
	CString str1 = _T("Content-Type:application/json;charset=utf-8");
	UnicodeToUTF8((LPCWSTR)str1, &pUTF8Header[0]);
	AUTODEL(pUTF8Header[0], utf81);
	LPCSTR pHeader[1];
	pHeader[0] = pUTF8Header[0];

	//Body
	char* pInfo(NULL);
	CString strJsonBody = _T("");
	UnicodeToUTF8((LPCWSTR)strJsonBody, &pInfo);
	AUTODEL(pInfo, uni3);

	//POST
	DataBlock headerData;
	DataBlock bodyData;
	
	DWORD dwS = GetTickCount();
	PostAndRecvWithHeader(pEncodedRequest, pHeader, headerData, bodyData, 0, pInfo);
	DWORD dwE = GetTickCount();

	//RESULT
	WCHAR* pUniTmp(NULL);
	UTF8ToUnicode((char*)bodyData.pBuff, &pUniTmp);
	AUTODEL(pUniTmp, unitmp);
	if (pUniTmp)
	{
		CString strResult = CString(pUniTmp);
		CString strLogInfo; strLogInfo.Format(_T("[LXCG_GETItemDataList]: Http返回结果: %s"), strResult);
	}
	json::Value my_data = json::Deserialize((char *)bodyData.pBuff);
	if (my_data.GetType() == json::NULLVal)
	{
		CString strLogInfo;strLogInfo.Format(_T("[LXCG_GETItemDataList]: %s"), _T("Http返回-BodyData类型为NULL..."));
		return FALSE;
	}
	
	//解析json
	......
	
	
	


*/

namespace http_toolkit
{
	//////////////////////////////////////////////////////////////////////////
	DataBlock::DataBlock(int nBufferSize)
	{
		nBufSize = nBufferSize;
		nPos = 0;
		pBuff = new BYTE[nBufferSize];
		memset(pBuff,0,nBufferSize);
	}

	DataBlock::~DataBlock()
	{
		delete []pBuff;
		nBufSize = 0;
		nPos = 0;
	}
	
	//////////////////////////////////////////////////////////////////////////
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

#define TOHEX(x) ((x)>9 ? (x)+55 : (x)+48)
	void URLEncode(char* szIn, char** pOut)
	{
		int nInLenth = strlen(szIn);
		int nFlag = 0;
		BYTE byte;
		*pOut = new char[nInLenth*3];
		char* szOut = *pOut;
		for (int i=0; i<nInLenth; i++)
		{
			byte = szIn[i];
			if (isalnum(byte) ||
				(byte == '-') ||  
				(byte == '_') ||   
				(byte == '.') ||   
				(byte == '~') ||
				(byte == '/') ||
				(byte == '?') ||
				(byte == ':') ||
				(byte == '=') ||
				(byte == '|') ||
				(byte == '&'))  
			{
				szOut[nFlag++] = byte;
			}
			else
			{
				if (isspace(byte))
				{
					szOut[nFlag++] = '+';
				}
				else if (byte == '%')
				{
					szOut[nFlag++] = byte;
					
					i++;byte = szIn[i];
					szOut[nFlag++] = byte;

					i++;byte = szIn[i];
					szOut[nFlag++] = byte;
				}
				else
				{
					szOut[nFlag++] = '%';
					szOut[nFlag++] = TOHEX(byte>>4);
					szOut[nFlag++] = TOHEX(byte%16);
				}
			}
		}
		szOut[nFlag] = '\0';
	}

	static size_t write_data_to_buf(void *ptr, size_t size, size_t nmemb, void *pUserData)
	{
		DataBlock *pDataBlock = (DataBlock *)pUserData;
		if (memcpy_s(pDataBlock->pBuff + pDataBlock->nPos, pDataBlock->nBufSize - pDataBlock->nPos, ptr, size * nmemb) != 0)
		{

			//TODO: error code;
			return 0;
		}

		pDataBlock->nPos += size * nmemb;
		return size * nmemb;
	}

	static size_t write_file(void *ptr, size_t size, size_t nmemb, void *pUserData)
	{
		HANDLE hFile = *((HANDLE *)pUserData);
		DWORD dwWrittenSize(0);
		::WriteFile(hFile, ptr, size * nmemb, &dwWrittenSize, NULL);

		return dwWrittenSize;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//GET
	BOOL SendAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData)
	{
		CURL *curl_handle;
		curl_global_init(CURL_GLOBAL_ALL);

		/* init the curl session */
		curl_handle = curl_easy_init();
		
		//curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT,3);  
		curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L); //强制设置请求方式为GET【Body有数据则会以POST进行请求】

		/* set URL to get */
		curl_easy_setopt(curl_handle, CURLOPT_URL, lpRequestUTF8Encoded);

		//header
		curl_slist *plist = curl_slist_append(NULL,lpHeader[0]);
		for(int i=1;i<nHeaderCount;i++)
		{
			curl_slist_append(plist,lpHeader[i]);
		}
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, plist);
		
		//body
		if(strlen(lpBodyData)>0)
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, lpBodyData);

		/* no progress meter please */
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);		

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data_to_buf);


		/* we want the headers be written to this file handle */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, &headerData);

		/* we want the body be written to this file handle instead of stdout */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, &bodyData);
		try
		{

			/* get it! */
			curl_easy_perform(curl_handle);

			curl_slist_free_all(plist);

			/* cleanup curl stuff */
			curl_easy_cleanup(curl_handle);

		}
		catch(...)
		{
			curl_slist_free_all(plist);

			/* cleanup curl stuff */
			curl_easy_cleanup(curl_handle);
		}

		return TRUE;
	}

	//POST
	BOOL PostAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData)
	{
		CURL *curl_handle;
		curl_global_init(CURL_GLOBAL_ALL);

		/* init the curl session */
		curl_handle = curl_easy_init();
		
		//curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT,3);   
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
		
		/* set URL to get */
		curl_easy_setopt(curl_handle, CURLOPT_URL, lpRequestUTF8Encoded);

		//header
		curl_slist *plist = curl_slist_append(NULL,lpHeader[0]);
		for(int i=1;i<nHeaderCount;i++)
		{
			curl_slist_append(plist,lpHeader[i]);
		}
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, plist);
		
		//body
		if(strlen(lpBodyData)>0)
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, lpBodyData);

		/* no progress meter please */
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);		

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data_to_buf);


		///* we want the headers be written to this file handle */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, &headerData);

		///* we want the body be written to this file handle instead of stdout */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, &bodyData);

		/* get it! */
		CURLcode res = curl_easy_perform(curl_handle);

		//获取HTTP错误码   
		long HTTP_flag = 0;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &HTTP_flag);
		if (HTTP_flag == 307)
		{


		}

		BOOL bRet = FALSE;
		if(res == CURLE_OK)
		{
			bRet = TRUE;
		}

		curl_slist_free_all(plist);

		/* cleanup curl stuff */
		curl_easy_cleanup(curl_handle);

		return bRet;
	}

	//DELETE
	BOOL DeleteAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData)
	{
		CURL *curl_handle;
		curl_global_init(CURL_GLOBAL_ALL);

		/* init the curl session */
		curl_handle = curl_easy_init();

		//curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT,3);   
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
		
		/* set URL to get */
		curl_easy_setopt(curl_handle, CURLOPT_URL, lpRequestUTF8Encoded);

		//header
		curl_slist *plist = curl_slist_append(NULL,lpHeader[0]);
		for(int i=1;i<nHeaderCount;i++)
		{
			curl_slist_append(plist,lpHeader[i]);
		}
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, plist);
		
		//body
		if(strlen(lpBodyData)>0)
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, lpBodyData);

		/* no progress meter please */
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);		

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data_to_buf);


		///* we want the headers be written to this file handle */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, &headerData);

		///* we want the body be written to this file handle instead of stdout */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, &bodyData);

		/* get it! */
		CURLcode res = curl_easy_perform(curl_handle);
		BOOL bRet = FALSE;
		if(res == CURLE_OK)
		{
			bRet = TRUE;
		}

		curl_slist_free_all(plist);

		/* cleanup curl stuff */
		curl_easy_cleanup(curl_handle);

		return bRet;
	}

	//PUT
	BOOL PutAndRecvWithHeaderBody(LPCSTR lpRequestUTF8Encoded,LPCSTR lpHeader[],DataBlock &headerData, DataBlock &bodyData,int nHeaderCount, LPCSTR lpBodyData)
	{
		CURL *curl_handle;
		curl_global_init(CURL_GLOBAL_ALL);

		/* init the curl session */
		curl_handle = curl_easy_init();

		//curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT,3);   
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
		
		/* set URL to get */
		curl_easy_setopt(curl_handle, CURLOPT_URL, lpRequestUTF8Encoded);

		//header
		curl_slist *plist = curl_slist_append(NULL,lpHeader[0]);
		for(int i=1;i<nHeaderCount;i++)
		{
			curl_slist_append(plist,lpHeader[i]);
		}
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, plist);
		
		//body
		if(strlen(lpBodyData)>0)
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, lpBodyData);

		/* no progress meter please */
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);		

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data_to_buf);


		///* we want the headers be written to this file handle */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, &headerData);

		///* we want the body be written to this file handle instead of stdout */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, &bodyData);

		/* get it! */
		CURLcode res = curl_easy_perform(curl_handle);
		BOOL bRet = FALSE;
		if(res == CURLE_OK)
			bRet = TRUE;

		curl_slist_free_all(plist);

		/* cleanup curl stuff */
		curl_easy_cleanup(curl_handle);

		return bRet;
	}

	//GET-from data
	BOOL DownloadFile(LPCSTR lpRequestUTF8Encoded, LPCTSTR lpStorePath, CString &strFileName, BOOL bReplace)
	{
		CURL *curl_handle;
		curl_global_init(CURL_GLOBAL_ALL);

		/* init the curl session */
		curl_handle = curl_easy_init();

		//https请求 不验证证书和hosts
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

		/* set URL to get */
		curl_easy_setopt(curl_handle, CURLOPT_URL, lpRequestUTF8Encoded);

		/* no progress meter please */
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

		/* send all data to this function  */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);

		if (strFileName.GetLength() == 0)
		{
			WCHAR *pRequestUnicode;
			UTF8ToUnicode((char *)lpRequestUTF8Encoded, &pRequestUnicode);

			CString strRequestUnicode =  pRequestUnicode;

			int nLastSlashPos = strRequestUnicode.ReverseFind(_T('/'));

			strFileName = strRequestUnicode.Mid(nLastSlashPos + 1);

			delete []pRequestUnicode;
			pRequestUnicode = NULL;

		}


		CString strFullName(lpStorePath);
		if (strFullName.GetLength() && 
			strFullName.ReverseFind('\\') != (strFullName.GetLength()-1))
		{
			strFullName += _T('\\');
		}
		strFullName += strFileName;
		if(!bReplace)
		{
			 WIN32_FIND_DATA fileInfo; 
			 HANDLE hFind; 
			 hFind = FindFirstFile(strFullName ,&fileInfo); 
			 if(hFind != INVALID_HANDLE_VALUE && fileInfo.nFileSizeLow > 0)
			 {
				 FindClose(hFind); 
				 return TRUE;
			 }	
 		}


		/*if (_taccess(strFullName, 0) != -1)
		{
			return TRUE;
		}*/
		
		HANDLE hFile	=	::CreateFile(strFullName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

		

//		DataBlock headerData;

		/* we want the headers be written to this file handle */
//		curl_easy_setopt(curl_handle,   CURLOPT_WRITEHEADER, &headerData);

		/* we want the body be written to this file handle instead of stdout */
		curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, &hFile);

		/* get it! */
		CURLcode code = curl_easy_perform(curl_handle);
		int nRetry = 0;
		while (code != 0)
		{
			CString strDbg;
			strDbg.Format(L"Code=%d;Retry=%d;File:%s",code,nRetry,strFullName);
			OutputDebugString(strDbg);
			if (nRetry < 3)
			{
				Sleep(10);
				code = curl_easy_perform(curl_handle);

			}
			else 
				break;
			
			nRetry++;
		}
		

		/* cleanup curl stuff */
		curl_easy_cleanup(curl_handle);

		CloseHandle(hFile);

//		::FlushFileBuffers(hFile);
		/*DWORD dwFileSize(0);
		::GetFileSize(hFile, &dwFileSize);
		::CloseHandle(hFile);

		if (dwFileSize == 0)
		{
			::DeleteFile(strFullName);
		}*/
		return TRUE;

	}

}

