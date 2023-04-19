#ifndef __I_GOSDDATAMGR_H__
#define __I_GOSDDATAMGR_H__

#define  GetBit(x, y)       (x &= (1 << y))     //Get No.y bit from x
#define  SetBit(x, y)       (x |= 1 << y)       //Set No.y bit in x is 1
#define  ClearBit(x, y)     (x &= ~(1 << y))    //Set No.y bit in x is 0

#include "GOSDItemNameDefine.h"
interface I_GOSDDataMgr 
{
	virtual BOOL  LoadGOSDFile(LPCTSTR lpszFilePath) = 0;
	virtual DWORD GetItemValue(CString strItemName) = 0;
	virtual void  SetItemValue(CString strItemName,DWORD dwValue) = 0;
	virtual void  GetYUV(COLORREF clr,int* nY,int* nU,int* nV) = 0;
	virtual COLORREF GetClrByYUV(int nY,int nU,int nV) = 0;
	virtual BOOL  ConvertImage(int nClusterNum,CString strSourcePath,CString strTargetPath) = 0;
};
#endif