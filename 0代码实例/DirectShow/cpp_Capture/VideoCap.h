// VideoCap.h: interface for the CVideoCap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOCAP_H__ECEA0BC1_051C_4821_8E00_67E52040295C__INCLUDED_)
#define AFX_VIDEOCAP_H__ECEA0BC1_051C_4821_8E00_67E52040295C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CAP_DEVICE_INFO
{
	CHAR	szDisplayName[MAX_PATH];
	CHAR	szFriendlyName[120];
	CHAR	szDevicePath[MAX_PATH];
};

typedef struct {
	UINT			nCount;
	CAP_DEVICE_INFO	*pInfo;
} CAP_DEVICE_LIST, *PCAP_DEVICE_LIST;

typedef struct {
	LONG lLocked;
	LONG lBufSize;
	LONG lBufUsed;
	BYTE *pbBuf;
} SAMPLE_DATA, *PSAMPLE_DATA;

#ifdef	__SIMULATOR__

#define SIMULATOR_THREAD_COUNT 2

class CSimulator_Process;

enum {
	INITIAL_STATE,
	RUNNING_STATE,
	COMPLETE_STATE,
	EXIT_STATE
};

typedef struct _SIMULATOR_THREAD {
	int  iThdID;
	int	 iState;
	int  iOutFrameID;
	int  iBufferSize;
	BOOL iThdBreak;
	BOOL iBufferUsed;
	BYTE *pYUY2Buffer;
	CWinThread* pThd;
	PVOID pParent;
	HANDLE hThdTrigger;
	HANDLE hCompleteTrigger;
	UINT (*pThdFunc)(LPVOID pParam); 
	CSimulator_Process *pSimulatorProcess;	
} SIMULATOR_THREAD, *PSIMULATOR_THREAD;

typedef struct _THREAD_FRAME_INFO {
	int iInFrameID;
	int iOutFrameID;
	int iThdID;
} THREAD_FRAME_INFO, *PTHREAD_FRAME_INFO;

#endif

class CVideoCap : public ISampleGrabberCB
{
public:
//	LPCSTR GetCapDevicePath() { return (m_pfVideo ? m_szDevicePath : NULL); };
	LPCSTR GetCapFriendlyName() { return (m_pfVideo ? m_szFriendlyName : NULL); };
	HRESULT GetBitmapInfoHeader(BITMAPINFOHEADER &bmiHeader);
	static HRESULT EnumDevices(CAP_DEVICE_LIST *pCapDeviceList);
	HRESULT ShowCapPinPropertyFrame(HWND hWndOwner);
	HRESULT ShowCapFilterPropertyFrame(HWND hWndOwner);
	HRESULT SetAttachWindow(HWND hWndPreview, HWND hWnd, BOOL bVisible = TRUE, BOOL bAutoResize = TRUE);
	HRESULT GetVideoWindow(IVideoWindow *&pVideoWindow);
	HRESULT SetNotifyWindow(HWND hWnd, LONG lMsg, LONG lInstanceData = 0);
	HRESULT TearDownGraph();
	HRESULT BuildPreviewGraph();
	HRESULT StopPreview();
	HRESULT StartPreview();
	HRESULT ChooseDevice(IMoniker *pmVideo);
	HRESULT ChooseDevice(LPCSTR pszDevice);
	HRESULT CreateFilterGraph();
	HRESULT RenderFilterGraph();
	static DWORD WINAPI StopThread(LPVOID lpParameter);
	void CreateStopThread();
    IBaseFilter* GetFilter( void );

	CVideoCap();
	virtual ~CVideoCap();

// ISampleGrabberCB Implementation
public:
	HRESULT GetKsProperty(
		REFGUID guidPropSet,
		DWORD dwPropID,
		LPVOID pInstanceData,
		DWORD cbInstanceData,
		LPVOID pPropData,
		DWORD cbPropData,
		DWORD *pcbReturned
	);
	HRESULT SetKsProperty(
		REFGUID guidPropSet,
		DWORD dwPropID,
		LPVOID pInstanceData,
		DWORD cbInstanceData,
		LPVOID pPropData,
		DWORD cbPropData
	);
	HRESULT GetLastSnapshot(PBYTE *ppbBuf, LONG *plBufSize);
	HRESULT MakeSnapshot(PBYTE *ppbBuf, LONG *plBufSize);
    // Fake out any COM ref counting
    //
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

    // Fake out any COM QI'ing
    //
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
    {
		if (!ppv) return E_POINTER;
        
        if(riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
            *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
            return NOERROR;
        }    

        return E_NOINTERFACE;
    }

    // We don't implement this one
    //
    STDMETHODIMP SampleCB(double SampleTime, IMediaSample * pSample)
    {
        return 0;
    }

    // The sample grabber is calling us back on its deliver thread.
    // This is NOT the main app thread!
    //
    STDMETHODIMP BufferCB(double SampleTime, BYTE * pBuffer, long BufferSize);
	IPin * GetPin(IBaseFilter *inFilter, BOOL inInput, const char *inPartialName);



	HRESULT GetconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir,IPin **ppPin);
	//获取过滤器上没有连接的引脚

	HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir,IPin **ppPin);
	//重载函数，获取过滤器上没有连接的引脚
	BITMAPINFOHEADER GetBMIHeader(){return m_bmiHeader;};

	IMediaEventEx *m_pME;

	BOOL bCloseWindow;
	BOOL m_bDestory;
	BOOL m_bIsMediaYUY2;	
	void SetParentPointer(CWnd* pParentWnd = NULL);
	HRESULT WriteXUCtrl(GUID Set, ULONG NodeID, USHORT bmControls, ULONG BufferLen, PBYTE pBuffer);
	HRESULT ReadXUCtrl(GUID Set, ULONG NodeID, USHORT bmControls, ULONG BufferLen, PBYTE pBuffer);

#ifdef	__SIMULATOR__

	SIMULATOR_THREAD m_thread[SIMULATOR_THREAD_COUNT]; 
	static UINT GetImageThd(LPVOID pParam);
	void SWAP(BYTE *Src, BYTE *Dst, BOOL isDstUsed, long BufferSize);
	BYTE * pTempBuffer;
	THREAD_FRAME_INFO m_FrameInfo;

#endif

protected:
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2 *m_pCGB;

	ISampleGrabber *m_pSG;
//	IMediaEventEx *m_pME;
	IVideoWindow *m_pVW;
	IMoniker *m_pmVideo;
	IBaseFilter *m_pfVideo;

	IBaseFilter *pST;//smart tree filter
	IBaseFilter *pAD;//avi dec filter
	IBaseFilter *pSGF;//sample graph filter
	IBaseFilter *pMD;//MJPEG dec filter
	ISampleGrabber *pSG;//sample graph interface
	DWORD m_dwGraphRegister;
	BOOL m_bPreviewing;
	BOOL m_bPreviewGraphBuilt;
	CHAR m_szFriendlyName[120];
//	CHAR m_szDevicePath[MAX_PATH];
	BITMAPINFOHEADER m_bmiHeader;
	SAMPLE_DATA m_lastSample;
	HANDLE m_hSnapshotTrigger;
	HANDLE m_hSnapshotReady;

	int m_iOldTime;
	int m_iNewTime;
	int m_iTimeBuf[20];
	double m_dAvgTime;

protected:
	HRESULT NukeDownstream(IBaseFilter *pFilter);
	HRESULT FreeCapFilter();
	HRESULT InitCapFilter();
};

#endif // !defined(AFX_VIDEOCAP_H__ECEA0BC1_051C_4821_8E00_67E52040295C__INCLUDED_)
