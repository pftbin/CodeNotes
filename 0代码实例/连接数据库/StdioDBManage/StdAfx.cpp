// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif
#include <stdio.h>
#include <atlimpl.cpp>

#define	FILE_LOG_PATH	 _T("Sobey_Stdio_DBManage.txt")
void TruncateErrLogFile()
{
	HANDLE hFind;
	int nOpenAttempts;
	WIN32_FIND_DATA findFileData;
	TCHAR lpszSystemPath[MAX_PATH],lpszFile[MAX_PATH];
	memset(lpszFile,0,MAX_PATH*sizeof(TCHAR));
	memset(lpszSystemPath,0,MAX_PATH*sizeof(TCHAR));
	GetSystemDirectory(lpszSystemPath, MAX_PATH);
	_stprintf(lpszFile,_T("%s\\%s"),lpszSystemPath,FILE_LOG_PATH);
	hFind = FindFirstFile(lpszFile,&findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	FindClose(hFind);
	if(findFileData.nFileSizeLow > 10*1000*1000)
	{
		nOpenAttempts = 0;
		while(TRUE)
		{
			if(DeleteFile(lpszFile))
				return ;
			::Sleep(20);
			nOpenAttempts++;
			if (nOpenAttempts < 5) 
				continue;
			return ;
		}
	}
}

STDMETHODIMP WriteLog(BSTR bstrErrMsg, BSTR bstrHostName, ULONG ulUID, BSTR bstrModuleName, BSTR bstrFileName, ULONG ulLine, BSTR bstrAuthorName, BSTR bstrRemark)
{
	HANDLE hFile; 
	SYSTEMTIME sdt;
	CComVariant vt;
	VARIANT varTime;
	int nOpenAttempts;
	DWORD dwError,nWritten,nCount;
	char lpszErrorText[20480];
	char lpszErrMsg[1024],lpszHostName[30],lpszModuleName[100],lpszFileName[100],lpszAuthorName[100],lpszRemark[15360],lpszDateTime[30];
	hFile = NULL;
	try
	{
		TruncateErrLogFile();
		VariantInit(&varTime);
		GetLocalTime(&sdt);
		SystemTimeToVariantTime(&sdt, &varTime.date);
		varTime.vt = VT_DATE;
		vt = varTime;
		vt.ChangeType(VT_BSTR);

		memset(lpszErrorText,0,20480);
		memset(lpszErrMsg,0,1024);
		memset(lpszHostName,0,30);
		memset(lpszModuleName,0,100);
		memset(lpszFileName,0,100);
		memset(lpszAuthorName,0,100);
		memset(lpszRemark,0,15360);
		memset(lpszDateTime,0,30);

		WideCharToMultiByte(CP_ACP, 0, bstrErrMsg, -1, lpszErrMsg,1023,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, bstrHostName, -1, lpszHostName,30,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, bstrModuleName, -1, lpszModuleName,100,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, bstrFileName, -1, lpszFileName,100,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, bstrAuthorName, -1, lpszAuthorName,100,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, bstrRemark, -1, lpszRemark,15360,NULL,NULL);
		WideCharToMultiByte(CP_ACP, 0, vt.bstrVal, -1, lpszDateTime,30,NULL,NULL);

		VariantClear(&varTime);

		sprintf(lpszErrorText,
"*****************************************************\r\n\
时间    : %s\r\n错误信息: %s\r\n主机名  : %s\r\n用户ID  : %d\r\n\
模块名  : %s\r\n文件名  : %s\r\n行号    : %d\r\n作者    : %s\r\n备注    : %s\r\n\r\n",
		lpszDateTime,lpszErrMsg,lpszHostName,ulUID,lpszModuleName,lpszFileName,ulLine,lpszAuthorName,lpszRemark);

		nCount = strlen(lpszErrorText);
		nOpenAttempts = 0;
		while(TRUE)
		{
			hFile = CreateFile(FILE_LOG_PATH, GENERIC_WRITE|CREATE_NEW,FILE_SHARE_WRITE, NULL, OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
				break;
			dwError = ::GetLastError();
			if(dwError == ERROR_SHARING_VIOLATION)
			{
				::Sleep(20);
				nOpenAttempts++;
				if (nOpenAttempts < 30) 
					continue;
			}
			//其余的错误先返回
			return E_FAIL;
		}
		dwError = ::SetFilePointer(hFile, 0, NULL, 0x2);
		if (dwError  != INVALID_SET_FILE_POINTER)//出错了就不写了
		{
			if (!::WriteFile(hFile, lpszErrorText, nCount, &nWritten, NULL))
			{
				//写入出错!
			}
			if (nWritten != nCount)
			{
				//可能是磁盘写满
			}
		}
	}
	catch(...)
	{
	}
	if(hFile&&hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hFile);

	return S_OK;
}