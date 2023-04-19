

#include "stdafx.h"
#include "samplegrab.h"


IBaseFilter *pGrabberFilter = NULL;
ISampleGrabber *pGrabber = NULL;

long pBufferSize = 0;
unsigned char* pBuffer = 0;

Gdiplus::Bitmap *pCapturedBitmap = 0;
unsigned int gWidth = 0;
unsigned int gHeight = 0;
unsigned int gChannels = 0;


int sgSetBitmapData(Gdiplus::Bitmap* pBitmap, const unsigned char* pData);
void sgFlipUpDown(unsigned char* pData);
void sgFreeMediaType(AM_MEDIA_TYPE& mt);


IBaseFilter* sgGetSampleGrabber()
{
	return pGrabberFilter;
}

void sgCloseSampleGrabber()
{
	if (pBuffer != 0) {
		delete[] pBuffer;
		pBuffer = 0;
		pBufferSize = 0;
	}

	if (pCapturedBitmap != 0) {
		::delete pCapturedBitmap;
		pCapturedBitmap = 0;
	}

	SAFE_RELEASE(pGrabberFilter);
	SAFE_RELEASE(pGrabber);

	gWidth = 0;
	gHeight = 0;
	gChannels = 0;
}

HRESULT sgAddSampleGrabber(IGraphBuilder *pGraph)
{
	// Create the Sample Grabber.
	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**) & pGrabberFilter);
	if (FAILED(hr)) {
		return hr;
	}
	hr = pGraph->AddFilter(pGrabberFilter, L"Sample Grabber");
	if (FAILED(hr)) {
		return hr;
	}

	pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	return hr;
}

HRESULT sgSetSampleGrabberMediaType()
{
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	HRESULT hr = pGrabber->SetMediaType(&mt);
	if (FAILED(hr)) {
		return hr;
	}
	hr = pGrabber->SetOneShot(FALSE);
	hr = pGrabber->SetBufferSamples(TRUE);
	return hr;
}

HRESULT sgGetSampleGrabberMediaType()
{
	AM_MEDIA_TYPE mt;
	HRESULT hr = pGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr)) {
		return hr;
	}

	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)mt.pbFormat;
	gChannels = pVih->bmiHeader.biBitCount / 8;
	gWidth = pVih->bmiHeader.biWidth;
	gHeight = pVih->bmiHeader.biHeight;

	sgFreeMediaType(mt);
	return hr;
}

Gdiplus::Bitmap *sgGetBitmap()
{        
	if (pGrabber == 0 || pBuffer == 0 || gChannels != 3)
		return 0;

	if (pCapturedBitmap == 0)                
		pCapturedBitmap = ::new Gdiplus::Bitmap(gWidth, gHeight, PixelFormat24bppRGB);        
	else if (gWidth != pCapturedBitmap->GetWidth() || gHeight != pCapturedBitmap->GetHeight()) {
		::delete pCapturedBitmap;
		pCapturedBitmap = ::new Gdiplus::Bitmap(gWidth, gHeight, PixelFormat24bppRGB);
	}

	if (pBufferSize != gWidth * gHeight * gChannels)
		return 0;

	if (sgSetBitmapData(pCapturedBitmap, pBuffer) == 0)
		return pCapturedBitmap;
	else
		return 0;
}

unsigned char* sgGrabData()
{
	HRESULT hr;

	if (pGrabber == 0)
		return 0;

	long Size = 0;
	hr = pGrabber->GetCurrentBuffer(&Size, NULL);
	if (FAILED(hr))
		return 0;
	else if (Size != pBufferSize) {
		pBufferSize = Size;
		if (pBuffer != 0)
			delete[] pBuffer;
		pBuffer = new unsigned char[pBufferSize];
	}

	hr = pGrabber->GetCurrentBuffer(&pBufferSize, (long*)pBuffer);
	if (FAILED(hr))
		return 0;
	else {
		sgFlipUpDown(pBuffer);
		return pBuffer;                
	}
}

long sgGetBufferSize()
{
	return pBufferSize;
}

int sgSetBitmapData(Gdiplus::Bitmap* pBitmap, const unsigned char* pData)
{
	Gdiplus::BitmapData bitmapData;
	bitmapData.Width = pBitmap->GetWidth();
	bitmapData.Height = pBitmap->GetHeight();
	bitmapData.Stride = 3 * bitmapData.Width;
	bitmapData.PixelFormat = pBitmap->GetPixelFormat();
	bitmapData.Scan0 = (VOID*)pData;
	bitmapData.Reserved = NULL;

	Gdiplus::Status s = pBitmap->LockBits(&Gdiplus::Rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight()),
		Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
		PixelFormat24bppRGB, &bitmapData);
	if (s == Gdiplus::Ok)
		pBitmap->UnlockBits(&bitmapData);

	return s;
}

void sgFlipUpDown(unsigned char* pData)
{
	unsigned char* scan0 = pData;
	unsigned char* scan1 = pData + ((gWidth * gHeight * gChannels) - (gWidth * gChannels));

	for (unsigned int y = 0; y < gHeight / 2; y++) {
		for (unsigned int x = 0; x < gWidth * gChannels; x++) {
			swap(scan0[x], scan1[x]);
		}
		scan0 += gWidth * gChannels;
		scan1 -= gWidth * gChannels;
	}

}

unsigned int sgGetDataWidth()
{
	return gWidth;
}

unsigned int sgGetDataHeight()
{
	return gHeight;
}

unsigned int sgGetDataChannels()
{
	return gChannels;
}

void sgFreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// Unecessary because pUnk should not be used, but safest.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

