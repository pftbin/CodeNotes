#pragma once
#include <string>
using namespace std;

#pragma warning(disable: 4996)

#define	 WM_DESTORY_CHROME		(WM_USER + 2000)
#define	 WM_POSTREQUEST_SAVE	(WM_USER + 2001)

static BOOL GetAppFolder(LPTSTR szFolder, int nBufSize)
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	int nLen = lstrlen(szPath);
	for (int i = nLen - 1; i >= 0; i--)
	{
		if (szPath[i] == _T('\\'))
		{
			szPath[i] = 0;
			break;
		}
	}
	if (lstrlen(szPath) >= nBufSize) return FALSE;
	lstrcpy(szFolder, szPath);
	return TRUE;
}

//同为Unicode
static void Char2CString(const char* pBuffer, CString& pWideString)
{
	//计算char *数组大小，以字节为单位，一个汉字占两个字节
	int charLen = strlen(pBuffer);
	//计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, NULL, 0);
	//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
	TCHAR* buf = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, buf, len);
	buf[len] = '\0'; //添加字符串结尾，注意不是len+1
	//将TCHAR数组转换为CString
	pWideString.Append(buf);
	//删除缓冲区
	delete[]buf;
}

static CString pcharToCString(char* pChar)
{
	int charLen = strlen(pChar); //计算pChar所指向的字符串大小，以字节为单位，一个汉字占两个字节  
	int len = MultiByteToWideChar(CP_ACP, 0, pChar, charLen, NULL, 0); //计算多字节字符的大小，按字符计算  
	wchar_t* pWChar = new wchar_t[len + 1]; //为宽字节字符数申请空间，  
	MultiByteToWideChar(CP_ACP, 0, pChar, charLen, pWChar, len); //多字节编码转换成宽字节编码  
	pWChar[len] = '\0';
	//将wchar_t数组转换为CString  
	CString str;
	str.Append(pWChar);
	return str;
}

static char* CStringTopchar(CString str, int& nLen)
{
	int n = str.GetLength();
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char* buf = new char[len + 1];   //以字节为单//宽字节编码转换成多字节编码
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), buf, len, NULL, NULL);
	buf[len] = '\0';   //多字节字符以'\0'结束
	nLen = len;
	return buf;
}

//UTF-8与Unicode转换
static void UTF8ToUnicode(char* pUTF8Src, WCHAR** ppUnicodeDst)
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

static void UnicodeToUTF8(const WCHAR* pUnicodeSrc, char** ppUTF8Dst)
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
		NULL);

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
		NULL);
}