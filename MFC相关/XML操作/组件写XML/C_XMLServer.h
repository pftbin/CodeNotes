#pragma once

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

static void UnicodeToUTF8(const WCHAR *pUnicodeSrc, char** ppUTF8Dst)
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

class C_XMLServer
{
public:
	C_XMLServer(void);
	virtual~C_XMLServer(void);

public:
	BOOL XML_SaveDeviceConfigInfo(DEVCONFIGITEM_VEC& inDeviceConfigVec,CString strXMLPath);
};
