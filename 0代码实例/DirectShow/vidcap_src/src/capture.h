
#ifndef CAPTURE_h
#define CAPTURE_h

// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY  WM_APP+1

void GetCaptureDevices(CComboBox& adaptersBox);
HRESULT StartCaptureVideo(HWND msgWindow, HWND prvWindow, unsigned int devIndex = 1); //devIndex = 1,2,3 ...
void StopCaptureVideo();

HRESULT HandleGraphEvent(void);
HRESULT ChangePreviewState(int nShow);


#endif