#pragma once
#include <streams.h>

#include "I_VideoReversal.h"

class CVideoReversal : public CTransformFilter
{
public:
	CVideoReversal(LPUNKNOWN punk, HRESULT *phr);
	virtual~CVideoReversal(void);

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

	HRESULT CheckInputType(const CMediaType *mtIn);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT DecideBufferSize(IMemAllocator * pAllocator, ALLOCATOR_PROPERTIES *pprop);
	HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
};

