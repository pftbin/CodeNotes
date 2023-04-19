// Helper.h: interface for the CHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELPER_H__E5234874_BC69_48C9_9DF0_6B27D2DFE4F8__INCLUDED_)
#define AFX_HELPER_H__E5234874_BC69_48C9_9DF0_6B27D2DFE4F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHelper  
{
public:
	CHelper();
	virtual ~CHelper();

public:
	static void RemoveGraphFromRot(DWORD pdwRegister);
	static HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	template <class T>
	static inline void IntfSafeRelease(T *&pIntf) {
		IUnknown *pUnknown = NULL;
        pUnknown = dynamic_cast<IUnknown *>(pIntf);
		if (pUnknown) {
			pUnknown->Release();
			pIntf = NULL;
		}
	};
};

#endif // !defined(AFX_HELPER_H__E5234874_BC69_48C9_9DF0_6B27D2DFE4F8__INCLUDED_)
