#include "D2DRenderer.h"


CD2DRenderer::CD2DRenderer(void)
	: m_bitmap(0), m_d2dFactory(0), m_hWndTarget(0), m_displayMode(KeepAspectRatio)
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_ID2D1Factory, (void **)&m_d2dFactory);
}

CD2DRenderer::~CD2DRenderer(void)
{
	SafeRelease(m_bitmap);
	SafeRelease(m_hWndTarget);
	SafeRelease(m_d2dFactory);
}

HRESULT CD2DRenderer::PrepareRenderTarget(BITMAPINFOHEADER& bih, HWND hWnd, bool bFlipHorizontally)
{
	m_pBitmapInfo = bih;
	m_hWnd = hWnd;	
	m_bFlipHorizontally = bFlipHorizontally;
	m_pitch = m_pBitmapInfo.biWidth * 4;

	DiscardResources();

	return CreateResources();
}

HRESULT CD2DRenderer::CreateResources()
{
	D2D1_PIXEL_FORMAT pixelFormat = 
	{
        DXGI_FORMAT_B8G8R8A8_UNORM,
        D2D1_ALPHA_MODE_IGNORE
    };

    D2D1_RENDER_TARGET_PROPERTIES renderTargetProps = 
	{
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        pixelFormat,
        0,
        0,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    };

	RECT rect;
	::GetClientRect(m_hWnd, &rect);

    D2D1_SIZE_U windowSize = 
	{
        rect.right - rect.left,
        rect.bottom - rect.top
    };
    
    D2D1_HWND_RENDER_TARGET_PROPERTIES hWndRenderTargetProps = 
	{
        m_hWnd,
        windowSize,
        D2D1_PRESENT_OPTIONS_IMMEDIATELY 
    };

	HR(m_d2dFactory->CreateHwndRenderTarget(renderTargetProps, hWndRenderTargetProps, &m_hWndTarget));
    
	//  (0,0) + --------> X
	//        |
	//        |
	//        |
	//        Y

	if(m_bFlipHorizontally)
	{
		// Flip the image around the X axis
		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F(1, 0,
			                                      0, -1,
												  0, 0);

		// Move it back into place
		D2D1::Matrix3x2F translate = D2D1::Matrix3x2F::Translation(0, windowSize.height);
		m_hWndTarget->SetTransform(scale * translate);
	}

	FLOAT dpiX, dpiY;
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    D2D1_BITMAP_PROPERTIES bitmapProps = 
	{
        pixelFormat,
        dpiX,
        dpiY
    };
    
    D2D1_SIZE_U bitmapSize = 
	{
		m_pBitmapInfo.biWidth,
		m_pBitmapInfo.biHeight
    };

	return m_hWndTarget->CreateBitmap(bitmapSize, bitmapProps, &m_bitmap);
}

void CD2DRenderer::DiscardResources()
{
	SafeRelease(m_bitmap);
	SafeRelease(m_hWndTarget);
}

static inline void ApplyLetterBoxing(D2D1_RECT_F& rendertTargetArea, D2D1_SIZE_F& frameArea)
{
	const float aspectRatio = frameArea.width / frameArea.height;

	const float targetW = fabs(rendertTargetArea.right - rendertTargetArea.left);
	const float targetH = fabs(rendertTargetArea.bottom - rendertTargetArea.top);

	float tempH = targetW / aspectRatio;	
		
	if(tempH <= targetH) // desired frame height is smaller than display height so fill black on top and bottom of display 
	{		
		float deltaH = fabs(tempH - targetH) / 2;
		rendertTargetArea.top += deltaH;
		rendertTargetArea.bottom -= deltaH;
	}
	else //desired frame height is bigger than display height so fill black on left and right of display 
	{
		float tempW = targetH * aspectRatio;	
		float deltaW = fabs(tempW - targetW) / 2;

		rendertTargetArea.left += deltaW;
		rendertTargetArea.right -= deltaW;
	}
}

HRESULT CD2DRenderer::DrawSample(const BYTE* pRgb32Buffer)
{
	CheckPointer(pRgb32Buffer, E_POINTER);

	if(!m_bitmap || !m_hWndTarget)
	{
		HR(CreateResources());
	}

	HR(m_bitmap->CopyFromMemory(NULL, pRgb32Buffer, m_pitch));
	
	if (!(m_hWndTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		D2D1_SIZE_U newSize = { rect.right, rect.bottom };
		D2D1_SIZE_U size = m_hWndTarget->GetPixelSize();

		if(newSize.height != size.height || newSize.width != size.width)
		{
			m_hWndTarget->Resize(newSize);
		}

		D2D1_RECT_F rectangle = D2D1::RectF(0, 0, newSize.width, newSize.height);

		if(m_displayMode == KeepAspectRatio)
		{
			ApplyLetterBoxing(rectangle, m_bitmap->GetSize());
		}

		m_hWndTarget->BeginDraw();

		m_hWndTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		m_hWndTarget->DrawBitmap(m_bitmap, rectangle);

		HRESULT hr = m_hWndTarget->EndDraw();
		if(hr == D2DERR_RECREATE_TARGET)
		{
			DiscardResources();
		}
	}

    return S_OK;
}

void CD2DRenderer::SetDisplayMode(DisplayMode mode)
{
	m_displayMode = mode;
}

DisplayMode CD2DRenderer::GetDisplayMode()
{
	return m_displayMode;
}
