// VideoCap.cpp: implementation of the CVideoCap class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <time.h>
#include "VideoCap.h"
#include <vector>
#include "Helper.h"

#ifdef	__SIMULATOR__
#include ".\Simulator\Simulator_Process.h" 
#endif	

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

vector<CSize> m_vtr;

// BITMAPINFOHEADER m_bmiHeader1;
// BYTE Buffer1[640*480*3
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef	__SIMULATOR__
int CSimulator_Process::SensorDataFormat = NOTSIMULATOR;
BOOL CSimulator_Process::FileImageRead = FALSE;
BOOL CSimulator_Process::SimOpenOK = FALSE;
CString CSimulator_Process::SimOpenName = "";
#endif	
#define IMAGE_WIDTH        640
#define IMAGE_HEIGHT	   480
#define IMG_SIZE_3M		   2048*1536
#define IMG_SIZE_5M		   2560*1920
#define MAX_IMG_SIZE	   IMG_SIZE_3M
long g_pTempBuffer[MAX_IMG_SIZE/2];
CVideoCap::CVideoCap()
{
	m_pGB = NULL;
	m_pCGB = NULL;
	m_pSG = NULL;
	m_pME = NULL;
	m_pVW = NULL;
	m_pmVideo = NULL;
	m_pfVideo = NULL;

	m_bPreviewing = FALSE;
	m_bPreviewGraphBuilt = FALSE;

	bCloseWindow = FALSE;
	m_bDestory = FALSE;

	m_bIsMediaYUY2 = TRUE; 

	memset(&m_bmiHeader, 0, sizeof(m_bmiHeader));
	memset(&m_lastSample, 0, sizeof(m_lastSample));

	m_hSnapshotTrigger = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSnapshotReady = CreateEvent(NULL, FALSE, FALSE, NULL);
    pST = NULL;
    pAD = NULL;
    pSGF = NULL;
    pMD = NULL;
    pSG = NULL;
    m_iOldTime = 0;
    m_iNewTime = 0;
    ZeroMemory(m_iTimeBuf, sizeof(m_iTimeBuf));
    m_dAvgTime = 0.0;
#ifdef	__SIMULATOR__

	for (int i=0; i<SIMULATOR_THREAD_COUNT; i++)
	{
		m_thread[i].pYUY2Buffer			= new BYTE[2000*2000*2];
		m_thread[i].iThdID				= i;
		m_thread[i].iBufferSize			= 0;
		m_thread[i].pParent				= this;
		m_thread[i].iThdBreak			= FALSE;
		m_thread[i].iBufferUsed			= FALSE;
		m_thread[i].iState				= INITIAL_STATE;
		m_thread[i].pThd				= NULL;
		m_thread[i].pThdFunc			= &CVideoCap::GetImageThd;
		m_thread[i].hThdTrigger			= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_thread[i].hCompleteTrigger	= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_thread[i].pThd				= AfxBeginThread(m_thread[i].pThdFunc, m_thread+i, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		m_thread[i].pSimulatorProcess	= new CSimulator_Process(); 
	}
	m_FrameInfo.iInFrameID = m_FrameInfo.iOutFrameID = m_FrameInfo.iThdID = 0;
	pTempBuffer = new BYTE[2000*2000*2];

#endif	

}

CVideoCap::~CVideoCap()
{
	m_bDestory = TRUE;
// 	CreateStopThread();
// 	TearDownGraph();
	FreeCapFilter();
	CHelper::IntfSafeRelease(m_pmVideo);

#ifdef	__SIMULATOR__

	for(int i=0; i<SIMULATOR_THREAD_COUNT; i++)
	{
		m_thread[i].iBufferUsed	= FALSE;
		m_thread[i].iThdBreak = TRUE;
		m_thread[i].iState = EXIT_STATE;
		::SetEvent(m_thread[i].hThdTrigger);
		Sleep(100);

		if (m_thread[i].hCompleteTrigger)
		{
			CloseHandle(m_thread[i].hCompleteTrigger);
			m_thread[i].hCompleteTrigger = NULL;
		}

		if (m_thread[i].hThdTrigger)
		{
			CloseHandle(m_thread[i].hThdTrigger);
			m_thread[i].hThdTrigger = NULL;
		}

		if (m_thread[i].pYUY2Buffer)
		{
			delete[] m_thread[i].pYUY2Buffer;
			m_thread[i].pYUY2Buffer = 0;
		}

		if(m_thread[i].pSimulatorProcess)
		{
			delete m_thread[i].pSimulatorProcess;
			m_thread[i].pSimulatorProcess = NULL;
		}
	}

	if(pTempBuffer)
	{
		delete [] pTempBuffer;
		pTempBuffer = NULL;
	}	

#endif	

	if (m_lastSample.pbBuf)
	{
		delete[] m_lastSample.pbBuf;
		m_lastSample.lBufSize = 0;
		m_lastSample.pbBuf = NULL;
	}

	if (m_hSnapshotTrigger)
	{
		CloseHandle(m_hSnapshotTrigger);
		m_hSnapshotTrigger = NULL;
	}
	
	if (m_hSnapshotReady)
	{
		CloseHandle(m_hSnapshotReady);
		m_hSnapshotReady = NULL;
	}
}

STDMETHODIMP CVideoCap::BufferCB(double /*SampleTime*/, BYTE * /*pBuffer*/, long /*BufferSize*/)
{
	return S_OK;
}

HRESULT CVideoCap::ChooseDevice(LPCSTR pszVideo)
{
    WCHAR wszVideo[MAX_PATH];
	CComPtr<IBindCtx> pBC;
	CComPtr<IMoniker> pmVideo;
	DWORD dwEaten;
	HRESULT hr = S_OK;
	int ret;

    ret = MultiByteToWideChar(CP_ACP, 0, pszVideo, -1, wszVideo, sizeof(wszVideo) / sizeof(wszVideo[0]));
	if (0 == ret) {
		hr = E_UNEXPECTED;
		return hr;
	}

	hr = CreateBindCtx(0, &pBC);
	if (FAILED(hr)) return hr;

	hr = MkParseDisplayName(pBC, wszVideo, &dwEaten, &pmVideo);
	if (FAILED(hr)) return hr;

    hr = ChooseDevice(pmVideo);

	return hr;
}

HRESULT CVideoCap::ChooseDevice(IMoniker *pmVideo)
{
	HRESULT hr = S_OK;

	if (m_pmVideo == pmVideo) {
		hr = S_OK;
		return hr;
	}

	CHelper::IntfSafeRelease(m_pmVideo);
	m_pmVideo = pmVideo;
	m_pmVideo->AddRef();

// 	hr = StopPreview();
// 	hr = TearDownGraph();
	hr = FreeCapFilter();

	hr = InitCapFilter();

	return hr;
}

IPin * CVideoCap::GetPin(IBaseFilter *inFilter, BOOL inInput, 
						 const char *inPartialName)
{

	PIN_DIRECTION direction = inInput ? PINDIR_INPUT : PINDIR_OUTPUT;
	IPin * foundPin = NULL;

	if (inFilter)
	{
		IEnumPins * pinEnum = NULL;
		if (SUCCEEDED(inFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			IPin * pin = NULL;
			ULONG fetchCount = 0;
			while (!foundPin && SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if (SUCCEEDED(pin->QueryPinInfo(&pinInfo)))
					{
						if (pinInfo.dir == direction)
						{
							// Ignore the pin name
							if (!inPartialName)
							{
								pin->AddRef();
								foundPin = pin;
							}
							else
							{
								char pinName[128];
								::WideCharToMultiByte(CP_ACP, 0, pinInfo.achName, 
									-1,	pinName, 128, NULL, NULL);
								if (::strstr(pinName, inPartialName))
								{
									pin->AddRef();
									foundPin = pin;
								}
							}
						}
						pinInfo.pFilter->Release();
					}
					pin->Release();
				}
			}
			pinEnum->Release();
		}
	}

	// We don't keep outstanding reference count
	if (foundPin)
	{
		foundPin->Release();
	}
	return foundPin;
}

HRESULT CVideoCap::GetconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, 
								   IPin **ppPin)
{
	*ppPin=0;
	IEnumPins *pEnum=0;
	IPin *pPin=0;
	HRESULT hr=pFilter->EnumPins(&pEnum);
	if(FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1,&pPin,NULL)==S_OK)
	{
		PIN_DIRECTION ThispinDir;
		pPin->QueryDirection(&ThispinDir);
		if(ThispinDir==PinDir)
		{
			IPin* pTemp=0;
			hr=pPin->ConnectedTo(&pTemp);
			if(SUCCEEDED(hr))
			{
				pEnum->Release();
				*ppPin=pPin;
				return S_OK;

			}
			else
			{
				pTemp->Release();
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	return S_FALSE;
}

HRESULT CVideoCap::GetUnconnectedPin(IBaseFilter *pFilter, 
									 PIN_DIRECTION PinDir, IPin **ppPin)
{

	*ppPin=0;
	IEnumPins *pEnum=0;
	IPin *pPin=0;
	HRESULT hr=pFilter->EnumPins(&pEnum);
	if(FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1,&pPin,NULL)==S_OK)
	{
		PIN_DIRECTION ThispinDir;
		pPin->QueryDirection(&ThispinDir);
		if(ThispinDir==PinDir)
		{
			IPin* pTemp=0;
			hr=pPin->ConnectedTo(&pTemp);
			if(SUCCEEDED(hr))
			{
				pTemp->Release();
			}
			else
			{
				pEnum->Release();
				*ppPin=pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	return S_FALSE;
}
HRESULT CVideoCap::StartPreview()
{
	CComQIPtr<IMediaControl> pMC(m_pGB);
	HRESULT hr = S_OK;

	if (m_bPreviewing) {
		hr = S_OK;
		return hr;
	}

	if (!m_bPreviewGraphBuilt) {
		hr = E_UNEXPECTED;
		return hr;
	}

	if (!pMC) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pMC->Run();
	if (SUCCEEDED(hr)) {
		m_bPreviewing = TRUE;
	}
	else {
		pMC->Stop();
		m_bPreviewing = FALSE;
	}

	return hr;
}

DWORD WINAPI CVideoCap::StopThread(LPVOID lpParameter)
{
	((CVideoCap*)lpParameter)->StopPreview();

	return 0;
}

void CVideoCap::CreateStopThread()
{
	::CreateThread(NULL, 0, StopThread, (LPVOID)this, 0, NULL);
}

HRESULT CVideoCap::StopPreview()
{
	CComQIPtr<IMediaControl> pMC(m_pGB);
	HRESULT hr = S_OK;

	if (!m_bPreviewing) {
		hr = S_OK;
		return hr;
	}

	if (!m_bPreviewGraphBuilt) {
		hr = E_UNEXPECTED;
		return hr;
	}

	if (!pMC) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pMC->Stop();
	if(bCloseWindow)	
	{
		m_bPreviewing = FALSE;
		return hr;
	}

	if (SUCCEEDED(hr)) {
		m_bPreviewing = FALSE;
	}

	return hr;
}

HRESULT CVideoCap::InitCapFilter()
{
	CComPtr<IGraphBuilder> pGB;
	CComPtr<ICaptureGraphBuilder2> pCGB;
	CComPtr<ISampleGrabber> pSG;
	CComPtr<IBaseFilter> pfVideo;
	CComPtr<IPropertyBag> pPropBag;
	HRESULT hr = S_OK;

	if (!m_pmVideo) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pGB.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = m_pmVideo->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);
	if (SUCCEEDED(hr)) {
		VARIANT varName;

		VariantInit(&varName);
		varName.vt = VT_BSTR;
		hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
		if (SUCCEEDED(hr)) {
			WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, m_szFriendlyName, sizeof(m_szFriendlyName), NULL, NULL);
			SysFreeString(varName.bstrVal);
		}
		VariantClear(&varName);
	}

	hr = m_pmVideo->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pfVideo);
	if (FAILED(hr)) return hr;

	hr = pCGB.CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = pCGB->SetFiltergraph(pGB);
	if (FAILED(hr)) return hr;

	hr = pGB->AddFilter(pfVideo, NULL);
	if (FAILED(hr)) return hr;

	hr = pSG.CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	CMediaType GrabType;

	GrabType.SetType(&MEDIATYPE_Video);
	GrabType.SetSubtype(&MEDIASUBTYPE_RGB24);

	hr = pSG->SetMediaType( &GrabType );

	pGB.CopyTo(&m_pGB);
	pCGB.CopyTo(&m_pCGB);
	pfVideo.CopyTo(&m_pfVideo);
	pSG.CopyTo(&m_pSG);

// 	g_pHWController = new HWController(m_pfVideo);

	return hr;
}

HRESULT CVideoCap::FreeCapFilter()
{
	HRESULT hr = S_OK;

// 	if (g_pHWController)
// 	{
// 		delete g_pHWController;
// 		g_pHWController = NULL;
// 	}
// 
	m_szFriendlyName[0] = 0;

	if (m_pGB != NULL)
		CHelper::IntfSafeRelease(m_pGB);
	if (m_pCGB != NULL)
		CHelper::IntfSafeRelease(m_pCGB);
	if (m_pfVideo != NULL)
		CHelper::IntfSafeRelease(m_pfVideo);
	if (m_pSG != NULL)
		CHelper::IntfSafeRelease(m_pSG);

	return hr;
}

int iOutWidth, iOutHeight;

HRESULT CVideoCap::BuildPreviewGraph()
{
	CComPtr<IAMStreamConfig> pSC;
	CComQIPtr<IMediaEventEx> pME;
	CComQIPtr<IVideoWindow> pVW;
    CComQIPtr<IBaseFilter> pGrabberBase;
	AM_MEDIA_TYPE *pMediaType;
	HRESULT hr = S_OK;
	if (m_bPreviewGraphBuilt) {
		hr = S_OK;
		return hr;
	}

	if (m_bPreviewing) {
		hr = E_UNEXPECTED;
		return hr;
	}

	if (!m_pGB || !m_pCGB || !m_pfVideo || !m_pSG) {
		hr = E_NOINTERFACE;
		return hr;
	}
	CMediaType GrabType;
	GrabType.SetType(&MEDIATYPE_Video);
	GrabType.SetSubtype(&MEDIASUBTYPE_RGB24);
	hr = m_pSG->SetMediaType( &GrabType );
	pGrabberBase = m_pSG;
	if (pGrabberBase) {
		hr = m_pGB->AddFilter(pGrabberBase, L"Grabber");
		if (FAILED(hr)) return hr;
	}
	hr = CreateFilterGraph();
	hr = RenderFilterGraph();
	if (hr != S_OK)
	{
		return hr;
	}
	if ((S_OK != hr) && (VFW_S_NOPREVIEWPIN != hr)) {
		return hr;
	}

    m_vtr.clear();
	hr = m_pCGB->GetFiltergraph(&m_pGB);
	hr = m_pCGB->FindInterface(&PIN_CATEGORY_CAPTURE,
		 		&MEDIATYPE_Video, m_pfVideo, IID_IAMStreamConfig, (void **)&pSC);
	int iCount = 0, iSize = 0;
	hr = pSC->GetNumberOfCapabilities(&iCount, &iSize);
	// Check the size to make sure we pass in the correct structure.
	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		// Use the video capabilities structure.
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			hr = pSC->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);

			if (pmtConfig->majortype == MEDIATYPE_Video)
			{
				 if (pmtConfig->subtype == MEDIASUBTYPE_YUY2)
				 {
					 //TRACE("%d",pmtConfig->subtype);
					 VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					 LONG lWidth = pVih->bmiHeader.biWidth;
					 LONG lHeight = pVih->bmiHeader.biHeight;

					 if ( (lWidth>0) && (lHeight>0) )
					 {
						 CSize sz;
						 sz.cx = lWidth;
						 sz.cy = lHeight;
						 m_vtr.push_back(sz);
					 }
					 DeleteMediaType(pmtConfig);
				 }
				 else if(pmtConfig->subtype == MEDIASUBTYPE_MJPG)//<.begin cb_81621 [1/15/2013 yu_dai]
				 {
					 //TRACE("%d",pmtConfig->subtype);
					 VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					 LONG lWidth = pVih->bmiHeader.biWidth;
					 LONG lHeight = pVih->bmiHeader.biHeight;

					 if ( (lWidth>0) && (lHeight>0) )
					 {
						 CSize sz;
						 sz.cx = lWidth;
						 sz.cy = lHeight;
						 m_vtr.push_back(sz);
					 }
					 DeleteMediaType(pmtConfig);// cb_81621 end.> [1/15/2013 yu_dai]
				 }
			}
		}
	}

	hr = pSC->GetFormat(&pMediaType);

	if (pMediaType->subtype == MEDIASUBTYPE_YUY2)
	{
		TRACE("MEDIASUBTYPE_YUY2\n");
		m_bIsMediaYUY2 = TRUE;	
	}
	else if (pMediaType->subtype == MEDIASUBTYPE_MJPG)
	{
		TRACE("MEDIASUBTYPE_MJPG\n");
		m_bIsMediaYUY2 = FALSE;	
	}

	if (SUCCEEDED(hr)) {
		if ((FORMAT_VideoInfo == pMediaType->formattype) &&
			(pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER)))
		{
			VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
			m_bmiHeader = pVih->bmiHeader;

			iOutWidth = m_bmiHeader.biWidth;
			iOutHeight = m_bmiHeader.biHeight;

// 			m_SnImgToolDlg->m_iCurrentWidth = m_bmiHeader.biWidth;
// 			m_SnImgToolDlg->m_iCurrentHeight = m_bmiHeader.biHeight;
		}

		if (pMediaType->cbFormat > 0) {
			CoTaskMemFree((PVOID)pMediaType->pbFormat);
		}
		if (pMediaType->pUnk) {
			pMediaType->pUnk->Release();
		}
		CoTaskMemFree((PVOID)pMediaType);
	}

	pME = m_pGB;
	pVW = m_pGB;
	if (pME && pVW) {
		
		pME.CopyTo(&m_pME);
		pVW.CopyTo(&m_pVW);
	}
	else {
		hr = E_UNEXPECTED;
		return hr;
	}
	m_pSG->SetOneShot(FALSE);
	m_pSG->SetBufferSamples(FALSE);
	m_pSG->SetCallback(this, 1);

	CHelper::AddGraphToRot(m_pGB, &m_dwGraphRegister);

	m_bPreviewGraphBuilt = TRUE;

	return hr;
}

HRESULT CVideoCap::TearDownGraph()
{
	HRESULT hr = S_OK;

	if (!m_bPreviewGraphBuilt) {
		hr = S_OK;
		return hr;
	}

	if (m_bPreviewing) {
		hr = E_UNEXPECTED;
		return hr;
	}

	if (m_pVW) {
		m_pVW->put_Owner(NULL);
		m_pVW->put_Visible(OAFALSE);
	}

	if (m_pME) {
		m_pME->SetNotifyWindow(NULL, 0, 0);
	}

	CHelper::IntfSafeRelease(m_pME);
	CHelper::IntfSafeRelease(m_pVW);

	if (m_pfVideo) {
		hr = NukeDownstream(m_pfVideo);
	}

    if(m_dwGraphRegister) {
		CHelper::RemoveGraphFromRot(m_dwGraphRegister);
        m_dwGraphRegister = 0;
    }

	m_bPreviewGraphBuilt = FALSE;

	return hr;
}

HRESULT CVideoCap::SetNotifyWindow(HWND hWnd, LONG lMsg, LONG lInstanceData)
{
	HRESULT hr = S_OK;

	if (!m_pME) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = m_pME->SetNotifyWindow((OAHWND)hWnd, lMsg, lInstanceData);

	return hr;
}

HRESULT CVideoCap::GetVideoWindow(IVideoWindow *&pVideoWindow)
{
	HRESULT hr = S_OK;

	if (!m_pVW) {
		pVideoWindow = NULL;
		hr = E_NOINTERFACE;
		return hr;
	}

	pVideoWindow = m_pVW;
	pVideoWindow->AddRef();

	return hr;
}


HRESULT CVideoCap::SetAttachWindow(HWND hWndPreview, HWND hWnd,BOOL bVisible, BOOL bAutoResize)
{
	HRESULT hr = S_OK;

	if (!m_pVW) {
		hr = E_NOINTERFACE;
		return hr;
	}

	if (hWndPreview) {
		if (!IsWindow(hWndPreview)) {
			hr = E_UNEXPECTED;
			return hr;
		}

		RECT rect;

		m_pVW->put_Owner((OAHWND)hWndPreview);

		m_pVW->put_MessageDrain(reinterpret_cast<OAHWND>(hWnd));

		m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
		if (bAutoResize && GetClientRect(hWndPreview, &rect)) {
			m_pVW->SetWindowPosition(0, 0, rect.right, rect.bottom);
		}
		m_pVW->put_Visible(bVisible);
	}
	else {
		m_pVW->put_Owner(NULL);
		m_pVW->put_Visible(bVisible);
	}

	return hr;
}

HRESULT CVideoCap::ShowCapFilterPropertyFrame(HWND hWndOwner)
{
	CComQIPtr<ISpecifyPropertyPages> pSpec(m_pfVideo);
    CAUUID cauuid;
	HRESULT hr = S_OK;

	if (!pSpec) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pSpec->GetPages(&cauuid);
	if (FAILED(hr)) return hr;

    hr = OleCreatePropertyFrame(hWndOwner, 0, 0, NULL, 1,
        (IUnknown **)&m_pfVideo, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);

	CoTaskMemFree(cauuid.pElems);

	return hr;
}

HRESULT CVideoCap::ShowCapPinPropertyFrame(HWND hWndOwner)
{
	CComPtr<IAMStreamConfig> pSC;
	CComQIPtr<ISpecifyPropertyPages> pSpec;
    CAUUID cauuid;
	HRESULT hr = S_OK;

	if (!m_pCGB || !m_pfVideo) {
		hr = E_NOINTERFACE;
		return hr;
	}

	CreateStopThread();
// 	TearDownGraph();

	m_pSG->SetMediaType( NULL);

	hr = m_pCGB->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, m_pfVideo, IID_IAMStreamConfig, (void **)&pSC);
	if (FAILED(hr)) {

		return hr;
	}

	pSpec = pSC;
	if (!pSpec) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pSpec->GetPages(&cauuid);
	if (FAILED(hr)) return hr;

    hr = OleCreatePropertyFrame(hWndOwner, 0, 0, NULL, 1,
        (IUnknown **)&(pSC.p), cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);

	CoTaskMemFree(cauuid.pElems);

	return hr;
}

HRESULT CVideoCap::NukeDownstream(IBaseFilter *pFilter)
{
	CComPtr<IEnumPins> pEnumPins;
	PIN_INFO pinInfo;
	ULONG cFetched;
	HRESULT hr = S_OK;

	if (!pFilter) {
		hr = S_OK;
		return hr;
	}

	hr = pFilter->EnumPins(&pEnumPins);
	pEnumPins->Reset();

	while (S_OK == hr) {
		CComPtr<IPin> pPin;
		hr = pEnumPins->Next(1, &pPin, &cFetched);
		if ((S_OK == hr) && pPin) {
			CComPtr<IPin> pPinTo;
			pPin->ConnectedTo(&pPinTo);
			if (pPinTo) {
				hr = pPinTo->QueryPinInfo(&pinInfo);
				if (SUCCEEDED(hr)) {
					if (PINDIR_INPUT == pinInfo.dir) {
						NukeDownstream(pinInfo.pFilter);
						m_pGB->Disconnect(pPinTo);
						m_pGB->Disconnect(pPin);
						m_pGB->RemoveFilter(pinInfo.pFilter);
					}
					CHelper::IntfSafeRelease(pinInfo.pFilter);
				}
			}
		}
	}

	return hr;
}

HRESULT CVideoCap::EnumDevices(CAP_DEVICE_LIST *pCapDeviceList)
{
	CComPtr<ICreateDevEnum> pDevEnum;
	CComPtr<IEnumMoniker> pEnumMoniker;
	CComPtr<IEnumMoniker> pEnumMoniker2;
	HRESULT hr = S_OK;
	HRESULT hr_2 = E_FAIL;

	if (!pCapDeviceList) {
		hr = E_INVALIDARG;
		return hr;
	}

	hr = pDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hr)) return hr;

	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	if (S_OK != hr) return hr;

	hr = pEnumMoniker->Clone(&pEnumMoniker2);
	if (FAILED(hr)) return hr;

	memset(pCapDeviceList, 0, sizeof(CAP_DEVICE_LIST));
	while (S_OK == pEnumMoniker->Skip(1)) {
		pCapDeviceList->nCount++;
	}

	if (0 == pCapDeviceList->nCount) {
		hr = S_FALSE;
		return hr;
	}

	pCapDeviceList->pInfo = new CAP_DEVICE_INFO[pCapDeviceList->nCount];
	if (!pCapDeviceList->pInfo) {
		hr = E_OUTOFMEMORY;
		return hr;
	}

	for (UINT i = 0; i < pCapDeviceList->nCount; ++i) {
		CComPtr<IMoniker> pMoniker;
		CComPtr<IPropertyBag> pPropBag;
		LPOLESTR strDisplayName;

		if (S_OK != pEnumMoniker2->Next(1, &pMoniker, NULL)) break;

		hr = pMoniker->GetDisplayName(0, 0, &strDisplayName);
		if (SUCCEEDED(hr)) {
			WideCharToMultiByte(CP_ACP, 0, strDisplayName, -1, pCapDeviceList->pInfo[i].szDisplayName, sizeof(pCapDeviceList->pInfo[i].szDisplayName), NULL, NULL);
		}

		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		if (SUCCEEDED(hr)) {
			VARIANT varName;

			VariantInit(&varName);
			varName.vt = VT_BSTR;
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr)) {
				WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, pCapDeviceList->pInfo[i].szFriendlyName, sizeof(pCapDeviceList->pInfo[i].szFriendlyName), NULL, NULL);
				SysFreeString(varName.bstrVal);
			}
			VariantClear(&varName);

			varName.vt = VT_BSTR;
			hr = pPropBag->Read(L"DevicePath", &varName, 0);
			if (SUCCEEDED(hr)) {
				WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, pCapDeviceList->pInfo[i].szDevicePath, sizeof(pCapDeviceList->pInfo[i].szDevicePath), NULL, NULL);
				SysFreeString(varName.bstrVal);
			}
			VariantClear(&varName);
			if (SUCCEEDED(hr)) {
				hr_2 = hr;
			}
		}
	}

	if (SUCCEEDED(hr_2)) {
		return hr_2;
	}
	else
	{
		return hr;
	}
}

HRESULT CVideoCap::GetBitmapInfoHeader(BITMAPINFOHEADER &bmiHeader)
{
	HRESULT hr = S_OK;

	bmiHeader = m_bmiHeader;

	return hr;
}

HRESULT CVideoCap::MakeSnapshot(PBYTE *ppbBuf, LONG *plBufSize)
{
	HRESULT hr = S_OK;
	DWORD dwWait;

	ResetEvent(m_hSnapshotReady);
	SetEvent(m_hSnapshotTrigger);

	dwWait = WaitForSingleObject(m_hSnapshotReady, 5000);
	if (WAIT_OBJECT_0 == dwWait) {
		hr = GetLastSnapshot(ppbBuf, plBufSize);
	}
	else {
		hr = E_UNEXPECTED;
	}

	return hr;
}

HRESULT CVideoCap::GetLastSnapshot(PBYTE *ppbBuf, LONG *plBufSize)
{
	HRESULT hr = S_OK;

	if (ppbBuf) {
		*ppbBuf = NULL;
		if (1L == InterlockedIncrement(&m_lastSample.lLocked))
		{
			*ppbBuf = new BYTE[m_lastSample.lBufUsed];
			if (*ppbBuf) {
				memcpy(*ppbBuf, m_lastSample.pbBuf, m_lastSample.lBufUsed);
			}
			else {
				hr = E_OUTOFMEMORY;
			}
		}
		InterlockedDecrement(&m_lastSample.lLocked);
	}

	if (plBufSize) {
		*plBufSize = m_lastSample.lBufUsed;
	}

	return hr;
}

HRESULT CVideoCap::GetKsProperty(
	REFGUID guidPropSet,
	DWORD dwPropID,
	LPVOID pInstanceData,
	DWORD cbInstanceData,
	LPVOID pPropData,
	DWORD cbPropData,
	DWORD *pcbReturned
)
{
	CComQIPtr<IKsPropertySet> pKsPropertySet(m_pfVideo);
	HRESULT hr = S_OK;

	if (!pKsPropertySet) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pKsPropertySet->Get(guidPropSet,
							dwPropID,
							pInstanceData,
							cbInstanceData,
							pPropData,
							cbPropData,
							pcbReturned
							);

	return hr;
}

HRESULT CVideoCap::SetKsProperty(
	REFGUID guidPropSet,
	DWORD dwPropID,
	LPVOID pInstanceData,
	DWORD cbInstanceData,
	LPVOID pPropData,
	DWORD cbPropData
)
{
	CComQIPtr<IKsPropertySet> pKsPropertySet(m_pfVideo);
	HRESULT hr = S_OK;

	if (!pKsPropertySet) {
		hr = E_NOINTERFACE;
		return hr;
	}

	hr = pKsPropertySet->Set(guidPropSet,
							dwPropID,
							pInstanceData,
							cbInstanceData,
							pPropData,
							cbPropData
							);

	return hr;
}


HRESULT CVideoCap::WriteXUCtrl(GUID Set, ULONG NodeID, USHORT bmControls, ULONG BufferLen, PBYTE pBuffer)
{
	HRESULT		hr = S_OK;
	KSP_NODE	ExtensionProp;
    ULONG		ulBytesReturned;
	CComQIPtr<IKsControl> pKsControl(m_pfVideo);

	if (!m_pfVideo) 
	{
		hr = E_NOINTERFACE;
		return hr;
	}

	ExtensionProp.Property.Set   = Set;
	ExtensionProp.Property.Id    = bmControls;
	ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET|KSPROPERTY_TYPE_TOPOLOGY;
	ExtensionProp.NodeId = NodeID;

	hr = pKsControl->KsProperty(
		(PKSPROPERTY) &ExtensionProp,
		sizeof(ExtensionProp),
		pBuffer,
		0xb/*BufferLen*/,
		&ulBytesReturned);

// 	if (hr != S_OK)
// 	{
// 		CString meg;
// 		meg.Format("pKsControl->KsProperty return 0x%x\n",hr);
// 		//AfxMessageBox(meg);
// 	}

	return hr;
}
HRESULT CVideoCap::CreateFilterGraph()
{
	HRESULT hr = S_FALSE;
	hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC, 
		IID_IBaseFilter, (void **)&pST);//create smart tree filter
	hr = CoCreateInstance(CLSID_AVIDec, NULL, CLSCTX_INPROC, 
		IID_IBaseFilter, (void **)&pAD);//create avi dec filter
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, 
		IID_IBaseFilter, (void **)&pSGF);//create sample graph filter
	hr = CoCreateInstance(CLSID_MjpegDec, NULL, CLSCTX_INPROC, 
		IID_IBaseFilter, (void **)&pMD);//create avi dec filter
	hr = m_pGB->AddFilter(pST, L"SmartTee");
	hr = m_pGB->AddFilter(pAD, L"AVIDec");
	hr = m_pGB->AddFilter(pMD, L"MjpegDec");
	hr = m_pGB->AddFilter(pSGF, L"SampleGrabber");
	return hr;
}
HRESULT CVideoCap::RenderFilterGraph()
{
	HRESULT hr = S_FALSE;
	IPin *pOut=NULL;// is used to connect
	IPin *pIn=NULL;
	hr=GetUnconnectedPin(m_pfVideo,PINDIR_OUTPUT,&pOut);
	hr=GetUnconnectedPin(pST,PINDIR_INPUT,&pIn);
	m_pGB->Connect(pOut, pIn);
	hr=GetUnconnectedPin(pST,PINDIR_OUTPUT,&pOut);
	hr=GetUnconnectedPin(pSGF,PINDIR_INPUT,&pIn);
	m_pGB->Connect(pOut, pIn);
	hr=GetUnconnectedPin(pST,PINDIR_OUTPUT,&pOut);
	hr = m_pGB->Render(pOut);
	pOut->Release();
	pIn->Release();
	return hr;
}
HRESULT CVideoCap::ReadXUCtrl(GUID Set, ULONG NodeID, USHORT bmControls, ULONG BufferLen, PBYTE pBuffer)
{
	HRESULT		hr=S_OK;
	KSP_NODE	ExtensionProp;
    ULONG		ulBytesReturned=0;
	CComQIPtr<IKsControl> pKsControl(m_pfVideo);

	if (!m_pfVideo) 
	{
		hr = E_NOINTERFACE;
		return hr;
	}

	ExtensionProp.Property.Set   = Set;
	ExtensionProp.Property.Id    = bmControls;
	ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET|KSPROPERTY_TYPE_TOPOLOGY;
	ExtensionProp.NodeId = NodeID;

	hr = pKsControl->KsProperty(
		(PKSPROPERTY) &ExtensionProp,
		sizeof(ExtensionProp),
		pBuffer,
		0xb/*BufferLen*/,
		&ulBytesReturned);

// 	if (FAILED(hr))
// 	{	
// 		CString meg;
// 		meg.Format("pKsControl->KsProperty return 0x%x\n",hr);
		//AfxMessageBox(meg);
// 	}

	return hr;
}

IBaseFilter* CVideoCap::GetFilter( void )
{
    return m_pfVideo;
}
