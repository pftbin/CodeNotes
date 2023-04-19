#pragma once
#include <map>



#include "I_GOSDDataMgr.h"
#include "GOSDItemNameDefine.h"
class C_GOSDDataMgr : public I_GOSDDataMgr
{
public:
	C_GOSDDataMgr(void);
	virtual ~C_GOSDDataMgr(void);

	virtual DWORD GetItemValue(CString strItemName);
	virtual void  SetItemValue(CString strItemName,DWORD dwValue);
	virtual void  GetYUV(COLORREF clr,int* nY,int* nU,int* nV);
	virtual COLORREF GetClrByYUV(int nY,int nU,int nV);
	virtual BOOL  ConvertImage(int nClusterNum,CString strSourcePath,CString strTargetPath);
	virtual BOOL  LoadGOSDFile(LPCTSTR lpszFilePath);

private:
	void	InitDataMap();

protected:
	//Custom
	COLORREF	m_dwColor0;
	COLORREF	m_dwColor1;
	COLORREF	m_dwColor2;
	COLORREF	m_dwColor3;
	DWORD       m_dwMaxSize;
	DWORD		m_dwPicMaxWidth;
	DWORD       m_dwPicMaxHeight;
	//Init
	DWORD		m_dwEnable;
	DWORD		m_dwColor0_Y;
	DWORD		m_dwColor0_U;
	DWORD		m_dwColor0_V;
	DWORD		m_dwColor1_Y;
	DWORD		m_dwColor1_U;
	DWORD		m_dwColor1_V;
	DWORD		m_dwColor2_Y;
	DWORD		m_dwColor2_U;
	DWORD		m_dwColor2_V;
	DWORD		m_dwColor3_Y;
	DWORD		m_dwColor3_U;
	DWORD		m_dwColor3_V;

	DWORD		m_dwRes_X;
	DWORD		m_dwRes_Y;
	DWORD		m_dwStart_X;
	DWORD		m_dwStart_Y;
	DWORD		m_dwSize_W;
	DWORD		m_dwSize_H;
	DWORD		m_dwGPIO_Sel;
	DWORD		m_dwTrig_Mode;
	DWORD		m_dwBit_Mode;
	DWORD		m_dwScale_Value;
	DWORD		m_dwBk_Tie_YUV;

	std::map<CString,DWORD> m_mapData;
};
