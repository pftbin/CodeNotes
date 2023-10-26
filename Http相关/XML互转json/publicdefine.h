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

//ͬΪUnicode
static void Char2CString(const char* pBuffer, CString& pWideString)
{
	//����char *�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�
	int charLen = strlen(pBuffer);
	//������ֽ��ַ��Ĵ�С�����ַ����㡣
	int len = MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, NULL, 0);
	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ķ��ֽ��ַ���С
	TCHAR* buf = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���
	MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, buf, len);
	buf[len] = '\0'; //����ַ�����β��ע�ⲻ��len+1
	//��TCHAR����ת��ΪCString
	pWideString.Append(buf);
	//ɾ��������
	delete[]buf;
}

static CString pcharToCString(char* pChar)
{
	int charLen = strlen(pChar); //����pChar��ָ����ַ�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�  
	int len = MultiByteToWideChar(CP_ACP, 0, pChar, charLen, NULL, 0); //������ֽ��ַ��Ĵ�С�����ַ�����  
	wchar_t* pWChar = new wchar_t[len + 1]; //Ϊ���ֽ��ַ�������ռ䣬  
	MultiByteToWideChar(CP_ACP, 0, pChar, charLen, pWChar, len); //���ֽڱ���ת���ɿ��ֽڱ���  
	pWChar[len] = '\0';
	//��wchar_t����ת��ΪCString  
	CString str;
	str.Append(pWChar);
	return str;
}

static char* CStringTopchar(CString str, int& nLen)
{
	int n = str.GetLength();
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
	char* buf = new char[len + 1];   //���ֽ�Ϊ��//���ֽڱ���ת���ɶ��ֽڱ���
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), buf, len, NULL, NULL);
	buf[len] = '\0';   //���ֽ��ַ���'\0'����
	nLen = len;
	return buf;
}

//UTF-8��Unicodeת��
static void UTF8ToUnicode(char* pUTF8Src, WCHAR** ppUnicodeDst)
{
	int nUnicodeLen;        //ת����Unicode�ĳ���

	//���ת����ĳ��ȣ��������ڴ�
	nUnicodeLen = MultiByteToWideChar(CP_UTF8,
		0,
		pUTF8Src,
		-1,
		NULL,
		0);

	nUnicodeLen += 1;
	*ppUnicodeDst = new WCHAR[nUnicodeLen];

	//תΪUnicode
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