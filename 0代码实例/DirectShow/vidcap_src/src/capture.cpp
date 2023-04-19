
#include "stdafx.h"
#include "capture.h"
#include "samplegrab.h"



//
// Global Data
//
enum PLAYSTATE {Stopped, Paused, Running, Init};

DWORD g_dwGraphRegister = 0;

IVideoWindow  * g_pVW = NULL;
IMediaControl * g_pMC = NULL;
IMediaEventEx * g_pME = NULL;
IGraphBuilder * g_pGraph = NULL;
ICaptureGraphBuilder2 * g_pCapture = NULL;
PLAYSTATE g_psCurrent = Stopped;

//
// Functions
//
void Msg(TCHAR *szFormat, ...);

HRESULT FindCaptureDevice(IBaseFilter** ppSrcFilter, unsigned int devIndex = 1);

HRESULT GetInterfaces(HWND hWnd);
void CloseInterfaces(void);
HRESULT SetupVideoWindow(HWND hWnd);
void ResizeVideoWindow(HWND hWnd);
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);



void GetCaptureDevices(CComboBox& adaptersBox)
{
	adaptersBox.ResetContent();

	// Create the System Device Enumerator.
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **) & pSysDevEnum);
	if (FAILED(hr)) {
		Msg(TEXT("CoCreateInstance() hr=0x%x"), hr);
		return;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK) {
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) {
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
				(void **) & pPropBag);
			if (SUCCEEDED(hr)) {
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr)) {
					adaptersBox.AddString(varName.bstrVal);
				}
				VariantClear(&varName);

				// To create an instance of the filter, do the following:
				IBaseFilter *pFilter;
				hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
					(void **) & pFilter);
				// Now add the filter to the graph.
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();

	adaptersBox.SetCurSel(0);
}

HRESULT StartCaptureVideo(HWND msgWindow, HWND prvWindow, unsigned int devIndex)
{
	if (devIndex == 0)
		devIndex = 1;

	HRESULT hr;
	IBaseFilter *pSrcFilter = NULL;

	// Get DirectShow interfaces
	hr = GetInterfaces(msgWindow);
	if (FAILED(hr)) {
		Msg(TEXT("Failed to get video interfaces!  hr=0x%x"), hr);
		return hr;
	}

	// Attach the filter graph to the capture graph
	hr = g_pCapture->SetFiltergraph(g_pGraph);
	if (FAILED(hr)) {
		Msg(TEXT("Failed to set capture filter graph!  hr=0x%x"), hr);
		return hr;
	}

	// Use the system device enumerator and class enumerator to find
	// a video capture/preview device, such as a desktop USB video camera.
	hr = FindCaptureDevice(&pSrcFilter);
	if (FAILED(hr)) {
		// Don't display a message because FindCaptureDevice will handle it
		return hr;
	}


	// Add Capture filter to our graph.
	hr = g_pGraph->AddFilter(pSrcFilter, L"Video Capture");
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't add the capture filter to the graph!  hr=0x%x\r\n\r\n")
			TEXT("If you have a working video capture device, please make sure\r\n")
			TEXT("that it is connected and is not being used by another application.\r\n\r\n")
			TEXT("The sample will now close."), hr);
		pSrcFilter->Release();
		return hr;
	}

	hr = sgAddSampleGrabber(g_pGraph);
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't add the SampleGrabber filter to the graph!  hr=0x%x"), hr);
		return hr;
	}
	hr = sgSetSampleGrabberMediaType();
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't set the SampleGrabber media type!  hr=0x%x"), hr);
		return hr;
	}
	IBaseFilter* pGrabber = sgGetSampleGrabber();


	// Render the preview pin on the video capture filter
	// Use this instead of g_pGraph->RenderFile
	hr = g_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
		pSrcFilter, pGrabber/*NULL*/, NULL);
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't render the video capture stream.  hr=0x%x\r\n")
			TEXT("The capture device may already be in use by another application.\r\n\r\n")
			TEXT("The sample will now close."), hr);
		pSrcFilter->Release();
		return hr;
	}

	hr = sgGetSampleGrabberMediaType();

	// Now that the filter has been added to the graph and we have
	// rendered its stream, we can release this reference to the filter.
	pSrcFilter->Release();

	// Set video window style and position
	hr = SetupVideoWindow(prvWindow);
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't initialize video window!  hr=0x%x"), hr);
		return hr;
	}

	// Start previewing video data
	hr = g_pMC->Run();
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't run the graph!  hr=0x%x"), hr);
		return hr;
	}

	// Remember current state
	g_psCurrent = Running;

	return S_OK;
}

void StopCaptureVideo()
{
	sgCloseSampleGrabber();
	CloseInterfaces();
}

HRESULT FindCaptureDevice(IBaseFilter** ppSrcFilter, unsigned int devIndex)
{
	HRESULT hr = S_OK;
	IBaseFilter* pSrc = NULL;
	IMoniker* pMoniker = NULL;
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pClassEnum = NULL;

	if (!ppSrcFilter) {
		return E_POINTER;
	}

	// Create the system device enumerator
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **) & pDevEnum);
	if (FAILED(hr)) {
		Msg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
	}

	// Create an enumerator for the video capture devices

	if (SUCCEEDED(hr)) {
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr)) {
			Msg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
		}
	}

	if (SUCCEEDED(hr)) {
		// If there are no enumerators for the requested type, then
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if (pClassEnum == NULL) {
			MessageBox(NULL, TEXT("No video capture device was detected.\r\n\r\n")
				TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
				TEXT("to be installed and working properly.  The sample will now close."),
				TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
			hr = E_FAIL;
		}
	}

	// Find the [devIndex] video capture device on the device list.
	// Note that if the Next() call succeeds but there are no monikers,
	// it will return S_FALSE (which is not a failure).  Therefore, we
	// check that the return code is S_OK instead of using SUCCEEDED() macro.

	if (SUCCEEDED(hr)) {
		for (unsigned int i = 0; i < devIndex; i++) {
			hr = pClassEnum->Next(1, &pMoniker, NULL);
			if (hr == S_FALSE) {
				Msg(TEXT("Unable to access video capture device!"));
				hr = E_FAIL;
				break;
			}
		}
	}

	if (SUCCEEDED(hr)) {
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **) & pSrc);
		if (FAILED(hr)) {
			Msg(TEXT("Couldn't bind moniker to filter object!  hr=0x%x"), hr);
		}
	}

	// Copy the found filter pointer to the output parameter.
	if (SUCCEEDED(hr)) {
		*ppSrcFilter = pSrc;
		(*ppSrcFilter)->AddRef();
	}

	SAFE_RELEASE(pSrc);
	SAFE_RELEASE(pMoniker);
	SAFE_RELEASE(pDevEnum);
	SAFE_RELEASE(pClassEnum);

	return hr;
}

HRESULT GetInterfaces(HWND hWnd)
{
	HRESULT hr;

	// Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **) & g_pGraph);
	if (FAILED(hr))
		return hr;

	// Create the capture graph builder
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) & g_pCapture);
	if (FAILED(hr))
		return hr;

	// Obtain interfaces for media control and Video Window
	hr = g_pGraph->QueryInterface(IID_IMediaControl, (LPVOID *) & g_pMC);
	if (FAILED(hr))
		return hr;

	hr = g_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) & g_pVW);
	if (FAILED(hr))
		return hr;

	hr = g_pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) & g_pME);
	if (FAILED(hr))
		return hr;

	// Set the window handle used to process graph events
	hr = g_pME->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);

	return hr;
}


void CloseInterfaces(void)
{
	// Stop previewing data
	if (g_pMC)
		g_pMC->StopWhenReady();

	g_psCurrent = Stopped;

	// Stop receiving events
	if (g_pME)
		g_pME->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);

	// Relinquish ownership (IMPORTANT!) of the video window.
	// Failing to call put_Owner can lead to assert failures within
	// the video renderer, as it still assumes that it has a valid
	// parent window.
	if (g_pVW) {
		g_pVW->put_Visible(OAFALSE);
		g_pVW->put_Owner(NULL);
	}

	// Release DirectShow interfaces
	SAFE_RELEASE(g_pMC);
	SAFE_RELEASE(g_pME);
	SAFE_RELEASE(g_pVW);
	SAFE_RELEASE(g_pGraph);
	SAFE_RELEASE(g_pCapture);
}

HRESULT SetupVideoWindow(HWND hWnd)
{
	HRESULT hr;

	// Set the video window to be a child of the main window
	hr = g_pVW->put_Owner((OAHWND)hWnd);
	if (FAILED(hr))
		return hr;

	// Set video window style
	hr = g_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;

	// Use helper function to position video window in client rect
	// of main application window
	ResizeVideoWindow(hWnd);

	// Make the video window visible, now that it is properly positioned
	hr = g_pVW->put_Visible(OATRUE);
	if (FAILED(hr))
		return hr;

	return hr;
}


void ResizeVideoWindow(HWND hWnd)
{
	// Resize the video preview window to match owner window size
	if (g_pVW) {
		RECT rc;

		// Make the preview video fill our window
		GetClientRect(hWnd, &rc);
		g_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
}


HRESULT ChangePreviewState(int nShow)
{
	HRESULT hr = S_OK;

	// If the media control interface isn't ready, don't call it
	if (!g_pMC)
		return S_OK;

	if (nShow) {
		if (g_psCurrent != Running) {
			// Start previewing video data
			hr = g_pMC->Run();
			g_psCurrent = Running;
		}
	} else {
		// Stop previewing video data
		hr = g_pMC->StopWhenReady();
		g_psCurrent = Stopped;
	}

	return hr;
}


HRESULT HandleGraphEvent(void)
{
	LONG evCode;
	LONG_PTR evParam1, evParam2;
	HRESULT hr = S_OK;

	if (!g_pME)
		return E_POINTER;

	while (SUCCEEDED(g_pME->GetEvent(&evCode, &evParam1, &evParam2, 0))) {
		//
		// Free event parameters to prevent memory leaks associated with
		// event parameter data.  While this application is not interested
		// in the received events, applications should always process them.
		//
		hr = g_pME->FreeEventParams(evCode, evParam1, evParam2);

		// Insert event processing code here, if desired
	}

	return hr;
}



void Msg(TCHAR *szFormat, ...)
{
	TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);

	// Use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	(void)StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);

	// Ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = TEXT('\0');

	MessageBox(NULL, szBuffer, TEXT("PlayCap Message"), MB_OK | MB_ICONERROR);
}
