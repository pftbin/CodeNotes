#pragma once
#include "ColorButton.h"
#include "I_GOSDDataMgr.h"
#include "GOSDItemNameDefine.h"
#include "afxwin.h"

#include <map>
#include <vector>

// C_GOSDParameter dialog

class C_GOSDParameterDlg : public CDialog
{
	DECLARE_DYNAMIC(C_GOSDParameterDlg)

public:
	C_GOSDParameterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C_GOSDParameterDlg();

	void SetModelMgr(I_GOSDDataMgr* pGOSDMgr);
	void SaveUItoMadel();
	void UpdateModeltoUI();//from GOSDMgr
	
// Dialog Data
	enum { IDD = IDD_DIALOG_PARAMETER };

protected:
	I_GOSDDataMgr*		m_pGOSGMgr;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonClr0();
	afx_msg void OnBnClickedButtonClr1();
	afx_msg void OnBnClickedButtonClr2();
	afx_msg void OnBnClickedButtonClr3();
	afx_msg void OnBnClickedButtonSetCenter();
	afx_msg void OnEnKillfocusEditResX();
	afx_msg void OnEnKillfocusEditResY();
	afx_msg void OnEnKillfocusEditStartx();
	afx_msg void OnEnKillfocusEditStarty();
	afx_msg void OnEnKillfocusEditSizew();
	afx_msg void OnEnKillfocusEditSizeh();
	afx_msg void OnCbnSelchangeComboBitmode2();
	afx_msg void OnEnChangeEditY0();
	afx_msg void OnEnChangeEditU0();
	afx_msg void OnEnChangeEditV0();
	afx_msg void OnEnChangeEditY1();
	afx_msg void OnEnChangeEditU1();
	afx_msg void OnEnChangeEditV1();
	afx_msg void OnEnChangeEditY2();
	afx_msg void OnEnChangeEditU2();
	afx_msg void OnEnChangeEditV2();
	afx_msg void OnEnChangeEditY3();
	afx_msg void OnEnChangeEditU3();
	afx_msg void OnEnChangeEditV3();
	afx_msg void OnBnClickedButtonSaveGosdparameters();
protected:
	//Init State
	UINT      m_nCheckEnable[16];
	DWORD     m_dwCheckCtrlID[16];
	void      SaveCheckCtrlID();
	void	  InitCheckEnable();
	void	  AddComBoxItem();
	std::map<DWORD,CString> m_mapCtrlTips;
	std::vector<DWORD>      m_vecCtrlID;
	void	  InitCtrlIDVector();
	void	  InitDescMap();
	void	  ShowDescByPoint(CPoint point);
	
	int		  Str2Int(CString strHexString,UINT uSysNum = 10);
	void	  UpdateClrGroup(int nMode);
	BOOL	  SaveConfigFile(CString strTxtFilePath);
	CString	  SelFolderPath();

public:
	CComboBox m_comBoxEnable;
	CComboBox m_comBoxTrigMode;
	CComboBox m_comBoxBitMode;
	CComboBox m_comBoxScale;
	CComboBox m_comBoxBKTieYUV;
	CButton m_Ckeck0;
	CButton m_Ckeck1;
	CButton m_Ckeck2;
	CButton m_Ckeck3;
	CButton m_Ckeck4;
	CButton m_Ckeck5;
	CButton m_Ckeck6;
	CButton m_Ckeck7;
	CButton m_Ckeck8;
	CButton m_Ckeck9;
	CButton m_Ckeck10;
	CButton m_Ckeck11;
	CButton m_Ckeck12;
	CButton m_Ckeck13;
	CButton m_Ckeck14;
	CButton m_Ckeck15;
	DWORD m_nLastResX;
	DWORD m_nLastResY;
	DWORD m_nLastStartX;
	DWORD m_nLastStartY;
	DWORD m_nLastSizeW;
	DWORD m_nLastSizeH;
	CString m_strResX;
	CString m_strResY;
	CString m_strStartX;
	CString m_strStartY;
	CString m_strSizeW;
	CString m_strSizeH;
	CString m_strY0;
	CString m_strU0;
	CString m_strV0;
	CString m_strY1;
	CString m_strU1;
	CString m_strV1;
	CString m_strY2;
	CString m_strU2;
	CString m_strV2;
	CString m_strY3;
	CString m_strU3;
	CString m_strV3;
	CButton m_btnSetCenter;
	CColorButton m_btnColor0;
	CColorButton m_btnColor1;
	CColorButton m_btnColor2;
	CColorButton m_btnColor3;	

	
};
