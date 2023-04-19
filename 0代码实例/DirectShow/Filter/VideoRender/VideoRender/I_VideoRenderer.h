#pragma once

// {269BA141-1FDE-494B-9024-453A17838B9F}
static const GUID CLSID_MyVideoRenderer = 
{ 0x269ba141, 0x1fde, 0x494b, { 0x90, 0x24, 0x45, 0x3a, 0x17, 0x83, 0x8b, 0x9f } };

// {34E5B77C-CCBA-4EC0-88B5-BABF6CF3A1D2}
static const GUID IID_IVideoRenderer = 
{ 0x34e5b77c, 0xccba, 0x4ec0, { 0x88, 0xb5, 0xba, 0xbf, 0x6c, 0xf3, 0xa1, 0xd2 } };

#define FILTER_NAME L"My VideoRenderer"

//DX9.0 SDK lib
#ifdef _DEBUG
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmbasd.lib")
#pragma comment(lib, "d2d1.lib")
#else
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "STRMBASE.lib")
#pragma comment(lib, "d2d1.lib")
#endif

enum DisplayMode
{
	KeepAspectRatio = 0,
	Fill = 1
};

DECLARE_INTERFACE_(IVideoRenderer, IUnknown)
{
	STDMETHOD(SetVideoWindow)(HWND hWnd) PURE;
	STDMETHOD_(void, SetDisplayMode)(DisplayMode) PURE;
	STDMETHOD_(DisplayMode, GetDisplayMode)(void) PURE;
};