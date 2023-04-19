#pragma once

#include "ColorSpaceConverter.h"
#include "D2DRenderer.h"
#include "VideoWindow.h"
#include "Dvdmedia.h"

#include "I_VideoRenderer.h"

class CVideoRender : public CBaseVideoRenderer, public IVideoRenderer
{
public:
	DECLARE_IUNKNOWN;

	CVideoRender(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CVideoRender(void);

	virtual HRESULT DoRenderSample(IMediaSample *pMediaSample);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT SetMediaType(const CMediaType *pmt);
	virtual HRESULT StartStreaming();

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr); 
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	STDMETHODIMP SetVideoWindow(HWND hWnd);
	STDMETHOD_(void, SetDisplayMode)(DisplayMode);
	STDMETHOD_(DisplayMode, GetDisplayMode)(void);

	void CreateDefaultWindow();

private:
	HWND m_hWnd;
	CD2DRenderer* m_renderer;
	HANDLE m_event;
	BITMAPINFOHEADER m_bmpInfo;
	CMediaType m_mediaType;
	CColorSpaceConverter* m_converter;
};

