#include "MySourceFilter.h"
#include <initguid.h>

#pragma comment(lib, "winmm")
#if _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif

//动态库工程自然也要有入口函数（固定格式）
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
					  DWORD  dwReason, 
					  LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
//组件就少不了注册与注销函数（固定格式）
STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);

}
STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);

}

//组件，就要有GUID（通过工具创建）
DEFINE_GUID(CLSID_MyFilter, 
			0x159386e0, 0x5193, 0x48ac, 0x8a, 0x57, 0x17, 0x88, 0xc7, 0x33, 0x40, 0xc1);

//以下是注册信息的模版，写了注释的地方是我们需要填写的，其他的采用默认
const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
	&MEDIATYPE_Video,       // Major type
	&MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudOpPin =
{
	L"Output",             
	FALSE,                 
	TRUE,                   
	FALSE,                  
	FALSE,                  
	&CLSID_NULL,           
	NULL,                  
	1,                      
	&sudOpPinTypes
};     

const AMOVIESETUP_FILTER sudBallax =
{
	&CLSID_MyFilter,			// Custom GUID
	L"MySourceFilter",			// Filter Name
	MERIT_DO_NOT_USE,       
	1,                      
	&sudOpPin         
};

// COM global table of objects in this dll
CFactoryTemplate g_Templates[] = {
	{ L"MySourceFilter"				 //Filter Name
	, &CLSID_MyFilter				 //Custom GUID
	, CMySourceFilter::CreateInstance//Filter CreateInstance
	, NULL
	, &sudBallax }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

//CMySourceFilter.cpp
CUnknown* CMySourceFilter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	CUnknown *punk = new CMySourceFilter(NAME("MySourceFilter"),lpunk,phr);
	if (punk == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
	return punk;
}

CMySourceFilter::CMySourceFilter(TCHAR *tszName,LPUNKNOWN lpunk, HRESULT *phr)
: CSource(tszName,lpunk,CLSID_MyFilter,phr)
{
	CMyOutputPin* pOutPin = new CMyOutputPin(phr,this);
	if (FAILED(*phr))
	{
		//if failed,remove it
		RemovePin(pOutPin);
		pOutPin->Release();
	}
}

//MyOutputPin.cpp
CMyOutputPin::CMyOutputPin(HRESULT *phr, CSource *pFilter)
: CSourceStream("MySourceFilter",phr,pFilter,L"Out")
, m_nWidth(0)
, m_nHeight(0)
, m_nImageSize(0)
, m_nCount(0)
{
	m_pData[0] = LoadBitmapFileToMemory("C://DirectShow//1.bmp",
		m_nWidth,m_nHeight,m_nImageSize);
	m_pData[1] = LoadBitmapFileToMemory("C://DirectShow//2.bmp",
		m_nWidth,m_nHeight,m_nImageSize);
	m_pData[2] = LoadBitmapFileToMemory("C://DirectShow//3.bmp",
		m_nWidth,m_nHeight,m_nImageSize);
}

CMyOutputPin::~CMyOutputPin(void)
{
	delete []m_pData[0];
	delete []m_pData[1];
	delete []m_pData[2];
}

HRESULT CMyOutputPin::GetMediaType(int iPosition, CMediaType *pmt)
{
	CheckPointer(pmt,E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	if(iPosition < 0)
	{
		return E_INVALIDARG;
	}
	// Have we run off the end of types?
	if(iPosition > 0)
	{
		return VFW_S_NO_MORE_ITEMS;
	}

	//给媒体类型申请Format的空间
	//填充每一个对象，主要是BITMAPINFOHEADER结构
	VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
	if(NULL == pvi)
		return(E_OUTOFMEMORY);

	ZeroMemory(pvi, sizeof(VIDEOINFO));
	pvi->bmiHeader.biBitCount = 24;
	pvi->bmiHeader.biHeight = m_nHeight;
	pvi->bmiHeader.biWidth = m_nWidth;
	pvi->bmiHeader.biSizeImage = m_nImageSize;
	pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biCompression = BI_RGB;
	pvi->bmiHeader.biClrImportant = 0;

	SetRectEmpty(&pvi->rcSource);
	SetRectEmpty(&pvi->rcTarget);

	pmt->SetType(&MEDIATYPE_Video);			//设置主媒体类型
	pmt->SetSubtype(&MEDIASUBTYPE_RGB24);   //设置子媒体类型
	pmt->SetFormatType(&FORMAT_VideoInfo);  //设置详细格式类型
	pmt->SetSampleSize(m_nImageSize);		//设置Sample的大小
	pmt->SetTemporalCompression(FALSE);

	return NOERROR;
}

HRESULT CMyOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);

	if (*(pMediaType->Type()) != MEDIATYPE_Video
		|| !(pMediaType->IsFixedSize()))
	{
		return E_INVALIDARG;
	}

	const GUID *SubType = pMediaType->Subtype();
	if (SubType == NULL)
	{
		return E_INVALIDARG;
	}
	if (*SubType != MEDIASUBTYPE_RGB24)
	{
		return E_INVALIDARG;
	}
	const GUID* FormatType = pMediaType->FormatType();
	if (FormatType == NULL)
	{
		return E_INVALIDARG;
	}
	if (*FormatType != FORMAT_VideoInfo)
	{
		return E_INVALIDARG;
	}

	VIDEOINFO* pvi = (VIDEOINFO*)pMediaType->Format();
	if (pvi == NULL)
	{
		return E_INVALIDARG;
	}
	if (pvi->bmiHeader.biBitCount != 24 || 
		pvi->bmiHeader.biWidth != m_nWidth ||
		pvi->bmiHeader.biHeight != m_nHeight)
	{
		return E_INVALIDARG;
	}

	return S_OK;
}

//协商Sample的大小
HRESULT CMyOutputPin::DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pIMemAlloc,E_POINTER);
	CheckPointer(pProperties,E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	HRESULT hr = NOERROR;

	VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
	//确定只有一个buffer
	pProperties->cBuffers = 1;
	//设置buffer的大小
	pProperties->cbBuffer = m_nImageSize;

	ASSERT(pProperties->cbBuffer);

	//设置属性页
	ALLOCATOR_PROPERTIES Actual;
	hr = pIMemAlloc->SetProperties(pProperties,&Actual);
	if(FAILED(hr))
	{
		return hr;
	}

	if(Actual.cbBuffer < pProperties->cbBuffer)
	{
		return E_FAIL;
	}

	ASSERT(Actual.cBuffers == 1);
	return NOERROR;
}

//填充Sample
HRESULT CMyOutputPin::FillBuffer(IMediaSample *pMediaSample)
{
	CheckPointer(pMediaSample,E_POINTER);
	BYTE* pData = NULL;
	long lDataSize = 0;

	//获得Sample中存放数据的地址
	pMediaSample->GetPointer(&pData);
	//取得Sample分配的内存大小
	lDataSize = pMediaSample->GetSize();

	ZeroMemory(pData,lDataSize);
	//把当前需要显示的数据拷贝到内存中
	CopyMemory(pData,m_pData[m_nCount%3],m_nImageSize);

	//设置时间戳
// 	REFERENCE_TIME start = TS_ONE * m_nCount;
// 	REFERENCE_TIME stop = TS_ONE + start;
//	pMediaSample->SetTime(&start,&stop);
	pMediaSample->SetTime(NULL,NULL);

	//准备下一帧数据
	m_nCount++;

	pMediaSample->SetSyncPoint(TRUE);	

	return NOERROR;
}

BYTE* CMyOutputPin::LoadBitmapFileToMemory(TCHAR* pFileName, int& nWidth, int& nHeight, int& nImageDataSize)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage( NULL, pFileName, IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );

	if(hBitmap == NULL)
		return NULL;

	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);

	BITMAP bmp;
	GetObject(hBitmap, sizeof(bmp), &bmp);

	BITMAPINFOHEADER bih = {0};//位图信息头
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度

	nImageDataSize = bmp.bmWidthBytes * bmp.bmHeight;
	byte * p = new byte[nImageDataSize];//申请内存保存位图数据
	GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, p,
		(LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据

	SelectObject(hDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hDC);

	nWidth = bmp.bmWidth;
	nHeight = bmp.bmHeight;

	return p;
}