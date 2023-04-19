#pragma once
#include <DShow.h>
//#include <qedit.h>
#include <assert.h> 
#include <InitGuid.h>

#pragma region CLSID

// {B98D13E7-55DB-4385-A33D-09FD1BA26338}  
static const GUID CLSID_LavSplitter_Source =   
{ 0xB98D13E7, 0x55DB, 0x4385, { 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38 } };  

//{EE30215D-164F-4A92-A4EB-9D4C13390F9F}  
static const GUID CLSID_LavVideoDecoder =   
{ 0xEE30215D, 0x164F, 0x4A92, { 0xA4, 0xEB, 0x9D, 0x4C, 0x13, 0x39, 0x0F, 0x9F } };  

//{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}  
static const GUID CLSID_LavAudioDecoder =   
{ 0xE8E73B6B, 0x4CB3, 0x44A4, { 0xBE, 0x99, 0x4F, 0x7B, 0xCB, 0x96, 0xE4, 0x91 } };

EXTERN_C const CLSID CLSID_SampleGrabber;
class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37") SampleGrabber;

#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include "qedit.h"



#pragma endregion

class DirectShowAPI
{
public:
	DirectShowAPI(void);
	~DirectShowAPI(void);
 
public:
	HRESULT RegGraphFilter(IUnknown *pUnkGraph, DWORD *pdwRegister);  
	HRESULT	UnRegGraphFilter(DWORD pdwRegister);

	HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWCHAR wszName, IBaseFilter **ppF);  
	HRESULT ConnectFilters( IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
	HRESULT GetUnconectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);  
	HRESULT ConnectFilters2(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest); 
};
