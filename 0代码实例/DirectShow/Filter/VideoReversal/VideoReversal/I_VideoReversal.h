#pragma once

// {0C195E06-4DA4-4BDD-8B85-8CA8C59413AB}
static const GUID CLSID_VideoReversal = 
{ 0xc195e06, 0x4da4, 0x4bdd, { 0x8b, 0x85, 0x8c, 0xa8, 0xc5, 0x94, 0x13, 0xab } };

#define FILTER_NAME L"VideoReversal"

//DX9.0 SDK lib
#ifdef _DEBUG
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "STRMBASE.lib")
#endif


//Custom Interface function
