#include "StdAfx.h"
#include "DirectShowAPI.h"

DirectShowAPI::DirectShowAPI(void)
{
}

DirectShowAPI::~DirectShowAPI(void)
{
}

HRESULT DirectShowAPI::RegGraphFilter(IUnknown *pUnkGraph, DWORD *pdwRegister)   
{  
	IMoniker * pMoniker = NULL;  
	IRunningObjectTable *pROT = NULL;  

	if (FAILED(GetRunningObjectTable(0, &pROT)))   
	{  
		return E_FAIL;  
	}  

	const size_t STRING_LENGTH = 256;  

	WCHAR wsz[STRING_LENGTH];  

	StringCchPrintfW(  
		wsz, STRING_LENGTH,   
		_T("FilterGraph %08x pid %08x"),   
		(DWORD_PTR)pUnkGraph,   
		GetCurrentProcessId()  
		);  

	HRESULT hr = CreateItemMoniker(_T("!"), wsz, &pMoniker);  
	if (SUCCEEDED(hr))   
	{  
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,  
			pMoniker, pdwRegister);  
		pMoniker->Release();  
	}  
	pROT->Release();  

	return hr;  
}  

HRESULT DirectShowAPI::UnRegGraphFilter(DWORD pdwRegister)  
{  
	IRunningObjectTable *pROT = NULL;  
	if(SUCCEEDED(GetRunningObjectTable(0,&pROT)))  
	{  
		pROT->Revoke(pdwRegister);  
		pROT->Release();
		return S_OK;
	}  
	return S_FALSE;
}  

HRESULT DirectShowAPI::AddFilterByCLSID( IGraphBuilder *pGraph, const GUID& clsid, LPCWCHAR wszName, IBaseFilter **ppF )  
{  
	if (!pGraph || !ppF)  
		return E_POINTER;  
	*ppF = 0;  
	IBaseFilter *pF = 0;  

	HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pF));  
	if (SUCCEEDED(hr))  
	{  
		hr = pGraph->AddFilter(pF, wszName);  
		if (SUCCEEDED(hr))  
			*ppF = pF;  
		else  
			pF->Release();  
	}  
	return hr;  
}  

HRESULT DirectShowAPI::ConnectFilters( IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)  
{  
	if ((pGraph == NULL)||(pSrc == NULL)||(pDest == NULL))  
		return E_POINTER;  

	IPin *pOut = 0;  
	HRESULT hr = GetUnconectedPin(pSrc,PINDIR_OUTPUT,&pOut);  
	if (FAILED(hr))  
		return hr;  
	IPin *pIn = 0;  
	hr = GetUnconectedPin(pDest,PINDIR_INPUT,&pIn);  
	if (FAILED(hr))  
		return hr;  

	hr = pGraph->Connect(pOut,pIn);  
	pIn->Release(); 
	pOut->Release();  
	return hr;  
}

HRESULT DirectShowAPI::ConnectFilters2( IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest )  
{  
	if ((pGraph == NULL)||(pOut == NULL)||(pDest == NULL))  
		return E_POINTER;  

#ifdef _DEBUG  
	PIN_DIRECTION PinDir;  
	pOut->QueryDirection(&PinDir);  
	assert(PinDir == PINDIR_OUTPUT);  
#endif // _DEBUG  

	//得到下级filter的输入pin  
	IPin *pIn = 0;  
	HRESULT hr = GetUnconectedPin(pDest,PINDIR_INPUT,&pIn);  
	if (FAILED(hr))  
		return hr;  

	hr = pGraph->Connect(pOut,pIn);  
	pIn->Release();  
	return hr;  
}  

HRESULT DirectShowAPI::GetUnconectedPin( IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin )  
{  
	*ppPin = 0;  
	IEnumPins *pEnum = 0;  
	IPin *pPin = 0;  

	HRESULT hr = pFilter->EnumPins(&pEnum);  
	if (FAILED(hr))  
	{  
		return hr;  
	}  
	hr = pEnum->Reset();  
	while (pEnum->Next(1,&pPin,NULL) == S_OK)  
	{  
		PIN_DIRECTION ThisPinDirection;  
		pPin->QueryDirection(&ThisPinDirection);  
		if (ThisPinDirection == PinDir)  
		{  
			IPin *pTemp = 0;  
			hr = pPin->ConnectedTo(&pTemp);  
			if (SUCCEEDED(hr))  
			{   
				pTemp->Release();  
			}   
			else  
			{  
				pEnum->Release();  
				*ppPin = pPin;  
				return S_OK;  
			}             
		}  
		pPin->Release();  
	}  
	pEnum->Release();  
	return E_FAIL;  
} 

