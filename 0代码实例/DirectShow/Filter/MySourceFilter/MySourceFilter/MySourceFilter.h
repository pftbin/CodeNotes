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
	//CSource�������඼ʹ��CSourceStream����������Ϊpin
	:public CSourceStream
{
public:
	CMyOutputPin(HRESULT *phr, CSource *pFilter);
	~CMyOutputPin(void);

	//�����������
	//����pMediaSample����Ҫ���ݵ���һ��Filter����pin������
	//��������䵽pMediaSample�о�����������Ĺ���
	HRESULT FillBuffer(IMediaSample *pMediaSample);

	//Э��ÿ��CMediaSample���ݿ�Ĵ�С
	HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
		ALLOCATOR_PROPERTIES *pProperties);

	//���ý������
	//��ö������ö��֧�ֵ�ý������ʱ���ô˺����õ�PIN֧�ֵ�ý������
	//�˺�������pmt�ĸ�����Ա����ˣ��ɴ˺��������ݾ���PIN֧��ʲôý������
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	//����Ƿ�֧�ֲ��������ý������
	HRESULT CheckMediaType(const CMediaType *pMediaType);

	//�����������ƽӿڣ���򵥵�ԴFilter����Ҫ��������
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
	{
		return E_FAIL;
	}

private:
	BYTE* m_pData[3];//�洢ͼƬ����
	int m_nWidth;//ͼƬ�Ŀ�
	int m_nHeight;//ͼƬ�ĸ�
	int m_nImageSize;//ͼƬ���ݵĴ�С
	int m_nCount;//�������������л�ͼƬ���ݵĻ�����
	static BYTE* LoadBitmapFileToMemory(TCHAR* pFileName, int& nWidth, int& nHeight, int& nImageDataSize);
};