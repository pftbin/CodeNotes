#pragma once

#include "streams.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include "I_VideoRenderer.h"
#include "atlbase.h"
#include <math.h>

template <typename T>
inline void SafeRelease(T& p)
{
    if (NULL != p)
    {
        p.Release();
        p = NULL;
    }
}

#define HR(x) if(FAILED(x)) { return x; }

class CD2DRenderer
{
public:
	CD2DRenderer(void);
	virtual ~CD2DRenderer(void);

	HRESULT PrepareRenderTarget(BITMAPINFOHEADER& bih, HWND hWnd, bool bFlipHorizontally);
	HRESULT DrawSample(const BYTE* pRgb32Buffer);
	void SetDisplayMode(DisplayMode mode);
	DisplayMode GetDisplayMode();

private:
	HRESULT CreateResources();
	void DiscardResources();

private:
	CComPtr<ID2D1Factory>           m_d2dFactory;
    CComPtr<ID2D1HwndRenderTarget>  m_hWndTarget;
	CComPtr<ID2D1Bitmap>            m_bitmap; 

	UINT32 m_pitch;
	DisplayMode m_displayMode;
	BITMAPINFOHEADER m_pBitmapInfo;
	bool m_bFlipHorizontally;
	HWND m_hWnd;
};


