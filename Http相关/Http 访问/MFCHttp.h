#include <afxinet.h>
#include <WinInet.h>
#include <string>
using namespace std;

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

//字符转换
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