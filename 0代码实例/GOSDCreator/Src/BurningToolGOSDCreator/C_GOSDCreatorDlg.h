#pragma once
#include <map>
#include <vector>
#include "afxwin.h"

#include "ColorButton.h"
#include "I_GOSDDataMgr.h"
#include "GOSDItemNameDefine.h"

#include "C_ShowImageDlg.h"

#define DF_PALLET_MAT		512
#define DF_END_CNT			4
#define DF_FIRST_CNT		16

#define WM_USER_UPDATE_PARAMETER  (WM_USER + 101)


// C_GOSDCreatorDlg dialog

class C_GOSDCreatorDlg : public CDialog
{
	DECLARE_DYNAMIC(C_GOSDCreatorDlg)

public:
	C_GOSDCreatorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C_GOSDCreatorDlg();
	
	void SetModelMgr(I_GOSDDataMgr* pGOSDMgr);		//Model Pointer
	void SaveUItoMadel();	
	void UpdateModeltoUI();

// Dialog Data
	enum { IDD = IDD_DIALOG_CREATOR };

protected:
	I_GOSDDataMgr*		m_pGOSGMgr;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonGen();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnCbnSelchangeComboBitMode();
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnBnClickedButtonSavegosd();
	afx_msg void OnBnClickedButtonApplygosd();
	afx_msg void OnBnClickedButtonColor0();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonColor3();
	afx_msg void OnBnClickedButtonEqual1();
	afx_msg void OnBnClickedButtonEqual2();
	
	BOOL    m_bMoveColor;
	DWORD   m_dwStartIdx;
	DWORD	m_dwEndIdx;
	C_ShowImageDlg m_ImageShow;
	void    MoveColorTest(MSG* pMsg);
	void	RedrawMouse();
	void	ShowButton(MSG* pMsg);
	void	InitBtnICON();
	void	ExChangeColorIdx();
	void	UpdateCtrlEnable();
	void	ReDrawImage();
	void	SaveBtnColor();
	void	UpdateBtnColor();

	void	DrawSrcImage();
	void	DrawCurrentImage();
	void	ClearDataAndUI();//call when reload success
	void	UpdateClrGroup(int nMode);

protected:
	int	m_nClr0YUV[3];
	int	m_nClr1YUV[3];
	int	m_nClr2YUV[3];
	int	m_nClr3YUV[3];
	void	SavetoYUV();
	BOOL	SavePreview2File(CString strFilePath);
	BOOL	SaveBinFile(CString strImagePath,CString strBinFilePath);
	CString SelFolderPath();

//Save Color Data
	COLORREF				m_nColorArrayOld[DF_END_CNT];//Last Color
	COLORREF				m_nColorArrayNew[DF_END_CNT];//UI Button Color
	COLORREF				m_nColorBackUp[DF_END_CNT];//First Color

	void					BackUpColor(); //ArrayOld to ArrayBack
	void					ReplaceColor();//ArrayNew to ArrayOld
	void					ReSetColor();  //ArrayBack to ArrayNew
	COLORREF				GetNewColor(Color oldColor);

	int						GetEncoderClsid(const WCHAR* format, CLSID* pClsid);//for save end file
	int						GetColorIndex(int nBitMode,Color color);//for save bin file

//Init 4 Color
	CString					m_strSourcePath;
	CString					m_strTargetPath;
	CString					m_strImageType;
	COLORREF				m_colorPallet[DF_PALLET_MAT];
	int						m_nColorCnt[DF_PALLET_MAT];
	std::map<int,int>		m_mapCntOrder;
	std::vector<COLORREF>	m_vecFirstColor;

	int			GetNearIdx(BYTE byR,BYTE byG,BYTE byB);       
	void		SaveFirstMaxCnt();
	void		GetFinalPallet(int nFinalCnt);
	COLORREF	GetCovertColor(BYTE byR,BYTE byG,BYTE byB);

//Image FileInfo
private:
	BOOL				m_bLoadConfig;
	BOOL				m_bLoadImage;   
	BOOL				m_bGenImage;
	BOOL				m_bPreViewImgae;
	BOOL				m_bApplytoPara;
	BOOL				m_bResetImage;

	int					m_nSourceWidth;
	int					m_nSourceHeight;
	int					m_nPreviewW;
	int					m_nPreviewH;
	float				m_fGOSDSize;
	int					m_nComboxIndex;
	int					m_nClrCnt;

	void				UpdateImageInfo();
	BOOL				CheckWHSize(int nWidth,int nHeight);
	BOOL				CheckGOSDSize(int nWidth,int nHeight,int nMode,int nMaxSize = 64);

//Dialog Ctrl Data
private:
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	int		m_nMaxSize;
	CString m_strGOSDSize;
	CComboBox m_comBoxBitMode;
	CButton m_btnLoad;
	CButton m_btnGen;
	CButton m_btnReset;
	CButton m_btnPreView;
	CColorButton m_btnColor0;
	CColorButton m_btnColor1;
	CColorButton m_btnColor2;
	CColorButton m_btnColor3;
	CButton m_btnApplytoPara;
	CButton m_btnSavetoPattren;
};
