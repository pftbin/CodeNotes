#include "VideoReversal.h"

CVideoReversal::CVideoReversal(LPUNKNOWN punk,HRESULT *phr)
	:CTransformFilter(FILTER_NAME, punk, CLSID_VideoReversal)
{
}

CVideoReversal::~CVideoReversal(void)
{
}

CUnknown* WINAPI CVideoReversal::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	ASSERT(phr);

	CVideoReversal *pNewObject = new CVideoReversal(punk, phr);

	if (pNewObject == NULL) {
		if (phr)
			*phr = E_OUTOFMEMORY;
	}

	return pNewObject;
}

HRESULT CVideoReversal::CheckInputType(const CMediaType *mtIn)
{
	if (mtIn->majortype != MEDIATYPE_Video ||
		mtIn->subtype != MEDIASUBTYPE_RGB24 ||
		mtIn->formattype != FORMAT_VideoInfo )
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	VIDEOINFO* pvi = (VIDEOINFO*)mtIn->Format();

	if (pvi->bmiHeader.biBitCount != 24)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CVideoReversal::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if (m_pInput->IsConnected() == FALSE) {
		return E_UNEXPECTED;
	}

	if (iPosition < 0) {
		return E_INVALIDARG;
	}

	if (iPosition > 0) {
		return VFW_S_NO_MORE_ITEMS;
	}

	CheckPointer(pMediaType,E_POINTER);
	*pMediaType = m_pInput->CurrentMediaType();

	return NOERROR;
}

HRESULT CVideoReversal::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	if (*mtIn == *mtOut)
	{
		return NOERROR;
	}

	return E_FAIL;
}

HRESULT CVideoReversal::DecideBufferSize(IMemAllocator * pAllocator, ALLOCATOR_PROPERTIES *pprop)
{
	if (m_pInput->IsConnected() == FALSE) {
		return E_UNEXPECTED;
	}

	CheckPointer(pAllocator,E_POINTER);
	CheckPointer(pprop,E_POINTER);
	HRESULT hr = NOERROR;

	pprop->cBuffers = 1;
	pprop->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
	ASSERT(pprop->cbBuffer);

	ALLOCATOR_PROPERTIES Actual;
	hr = pAllocator->SetProperties(pprop,&Actual);
	if (FAILED(hr)) {
		return hr;
	}

	ASSERT( Actual.cBuffers == 1 );

	if (pprop->cBuffers > Actual.cBuffers ||
		pprop->cbBuffer > Actual.cbBuffer) {
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CVideoReversal::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	CheckPointer(pIn,E_POINTER);
	CheckPointer(pOut,E_POINTER);

	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pIn->GetActualDataLength();

	pIn->GetPointer(&pSourceBuffer);
	pOut->GetPointer(&pDestBuffer);

	//Reversal
	CMediaType pMediaType1 = m_pInput->CurrentMediaType();
	VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)pMediaType1.pbFormat;
	int nWidth = WIDTHBYTES(pvi->bmiHeader.biWidth * pvi->bmiHeader.biBitCount);
	for (int i = 0; i < pvi->bmiHeader.biHeight; i ++)
	{
		CopyMemory((PVOID) (pDestBuffer + nWidth * i),
			(PVOID) (pSourceBuffer + nWidth * (pvi->bmiHeader.biHeight - i - 1)),
			nWidth);
	}	 

	REFERENCE_TIME TimeStart, TimeEnd;
	if(NOERROR == pIn->GetTime(&TimeStart, &TimeEnd))
	{
		pOut->SetTime(&TimeStart, &TimeEnd);
	}

	LONGLONG MediaStart, MediaEnd;
	if(pIn->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR)
	{
		pOut->SetMediaTime(&MediaStart,&MediaEnd);
	}

	HRESULT hr = pIn->IsSyncPoint();
	if(hr == S_OK)
	{
		pOut->SetSyncPoint(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pOut->SetSyncPoint(FALSE);
	}
	else
	{
		return E_UNEXPECTED;
	}

	hr = pIn->IsPreroll();
	if(hr == S_OK)
	{
		pOut->SetPreroll(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pOut->SetPreroll(FALSE);
	}
	else
	{ 
		return E_UNEXPECTED;
	}

	hr = pIn->IsDiscontinuity();

	if(hr == S_OK)
	{
		pOut->SetDiscontinuity(TRUE);
	}
	else if(hr == S_FALSE)
	{
		pOut->SetDiscontinuity(FALSE);
	}
	else
	{
		return E_UNEXPECTED;
	}

	long lDataLength = pIn->GetActualDataLength();
	pOut->SetActualDataLength(lDataLength);

	return NOERROR;
}
