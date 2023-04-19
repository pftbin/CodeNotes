#pragma once
#include <streams.h>

class CMySourceFilter :	public CSource			
{
public:
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
private:
	CMySourceFilter(TCHAR *tszName,LPUNKNOWN lpunk, HRESULT *phr);	
};

//MyOutputPin.h
class CMyOutputPin 
	//CSource的派生类都使用CSourceStream的派生类做为pin
	:public CSourceStream
{
public:
	CMyOutputPin(HRESULT *phr, CSource *pFilter);
	~CMyOutputPin(void);

	//填充样本函数
	//参数pMediaSample就是要传递到下一个Filter输入pin的样本
	//把数据填充到pMediaSample中就是这个函数的功能
	HRESULT FillBuffer(IMediaSample *pMediaSample);

	//协商每个CMediaSample数据块的大小
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
		ALLOCATOR_PROPERTIES *pProperties);

	//获得媒体类型
	//在枚举器中枚举支持的媒体类型时调用此函数得到PIN支持的媒体类型
	//此函数设置pmt的各个成员，因此，由此函数的内容觉得PIN支持什么媒体类型
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	//检测是否支持参数传入的媒体类型
	HRESULT CheckMediaType(const CMediaType *pMediaType);

	//这是质量控制接口，最简单的源Filter不需要质量控制
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
	{
		return E_FAIL;
	}

private:
	BYTE* m_pData[3];//存储图片数据
	int m_nWidth;//图片的宽
	int m_nHeight;//图片的高
	int m_nImageSize;//图片数据的大小
	int m_nCount;//计数器，用来切换图片数据的缓冲区
	static BYTE* LoadBitmapFileToMemory(TCHAR* pFileName, int& nWidth, int& nHeight, int& nImageDataSize);
};