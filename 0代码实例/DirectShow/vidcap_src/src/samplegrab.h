
#ifndef SAMPLEGRAB_h
#define SAMPLEGRAB_h

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

IBaseFilter* sgGetSampleGrabber();
HRESULT sgAddSampleGrabber(IGraphBuilder *pGraph);
HRESULT sgSetSampleGrabberMediaType();
HRESULT sgGetSampleGrabberMediaType();

unsigned char* sgGrabData();            //call grab data first
Gdiplus::Bitmap* sgGetBitmap();        //fill bitmap with grabbed data
long sgGetBufferSize();

unsigned int sgGetDataWidth();
unsigned int sgGetDataHeight();
unsigned int sgGetDataChannels();

void sgCloseSampleGrabber();

#endif