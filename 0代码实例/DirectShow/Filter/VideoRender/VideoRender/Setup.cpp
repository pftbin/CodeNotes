#include <olectl.h>
#include <initguid.h>
#include "VideoRender.h"

#pragma warning(disable:4710)  // 'function': function not inlined (optimzation)

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudOpPin =
{
    L"Input",               // Pin string name
    TRUE,                   // Is it rendered
    FALSE,                  // Is it an output
    FALSE,                  // Can we have none
    FALSE,                  // Can we have many
    &CLSID_NULL,            // Connects to filter
    NULL,                   // Connects to pin
    1,                      // Number of types
    &sudOpPinTypes };       // Pin details

const AMOVIESETUP_FILTER sudBallax =
{
	&CLSID_MyVideoRenderer, // Filter CLSID
	FILTER_NAME,            // String name
    MERIT_DO_NOT_USE,       // Filter merit
    1,                      // Number pins
    &sudOpPin               // Pin details
};

CFactoryTemplate g_Templates[] = 
{
  { 
	FILTER_NAME,
    &CLSID_MyVideoRenderer,
    CVideoRender::CreateInstance,
    NULL,
    &sudBallax 
  }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
} 

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}