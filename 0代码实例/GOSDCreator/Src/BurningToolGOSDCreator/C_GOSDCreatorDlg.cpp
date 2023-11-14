// C_GOSDCreatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "C_GOSDCreatorDlg.h"

#include <math.h>
#include <set>
#include "C_MyColorDlg.h"
// C_GOSDCreatorDlg dialog

IMPLEMENT_DYNAMIC(C_GOSDCreatorDlg, CDialog)

C_GOSDCreatorDlg::C_GOSDCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C_GOSDCreatorDlg::IDD, pParent)
	, m_pGOSGMgr(NULL)
	, m_bLoadConfig(FALSE)
	, m_bLoadImage(FALSE)
	, m_bGenImage(FALSE)
	, m_bPreViewImgae(FALSE)
	, m_bApplytoPara(FALSE)
	, m_bResetImage(FALSE)
	, m_nSourceWidth(0)
	, m_nSourceHeight(0)
	, m_nPreviewW(0)
	, m_nPreviewH(0)
	, m_fGOSDSize(0.0)
	, m_nComboxIndex(0)
	, m_nClrCnt(2)
	, m_bMoveColor(FALSE)
	, m_dwEndIdx(0)
	, m_dwStartIdx(0)
	, m_nMaxSize(64)
	, m_dwWidth(0)
	, m_dwHeight(0)
	, m_strGOSDSize(_T("0 KByte"))
	, m_strSourcePath(_T(""))
	, m_strTargetPath(_T(""))
	, m_strImageType(_T("image/bmp"))//default ImageFile Type
{
	memset(m_colorPallet,0,sizeof(COLORREF)*DF_PALLET_MAT);
	memset(m_nColorCnt,0,sizeof(int)*DF_PALLET_MAT);
	memset(m_nColorArrayOld,0,sizeof(COLORREF)*DF_END_CNT);
	memset(m_nColorArrayNew,0,sizeof(COLORREF)*DF_END_CNT);

	//Ready Pallet             512
	for (int i=0;i<8;i++)//R
	{
		for (int j=0;j<8;j++)//G
		{
			for (int k=0;k<8;k++)//B
			{
				int nIndex = i*64+j*8+k;
				BYTE byR = i*36;
				BYTE byG = j*36;
				BYTE byB = k*36;

				m_colorPallet[nIndex] = RGB(byR,byG,byB);
			}
		}
	}
	
	memset(m_nClr0YUV,0,sizeof(int)*3);
	memset(m_nClr1YUV,0,sizeof(int)*3);
	memset(m_nClr2YUV,0,sizeof(int)*3);
	memset(m_nClr3YUV,0,sizeof(int)*3);
}

C_GOSDCreatorDlg::~C_GOSDCreatorDlg()
{
}

void C_GOSDCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_dwWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dwHeight);
	DDX_Text(pDX, IDC_EDIT_GOSD_SIZE, m_strGOSDSize);
	DDX_Control(pDX, IDC_COMBO_BIT_MODE, m_comBoxBitMode);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_btnLoad);
	DDX_Control(pDX, IDC_BUTTON_GEN, m_btnGen);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_btnReset);
	DDX_Control(pDX, IDC_BUTTON_PREVIEW, m_btnPreView);
	DDX_Control(pDX, IDC_BUTTON_COLOR0, m_btnColor0);
	DDX_Control(pDX, IDC_BUTTON_COLOR1, m_btnColor1);
	DDX_Control(pDX, IDC_BUTTON_COLOR2, m_btnColor2);
	DDX_Control(pDX, IDC_BUTTON_COLOR3, m_btnColor3);
	DDX_Control(pDX, IDC_BUTTON_APPLYGOSD, m_btnApplytoPara);
	DDX_Control(pDX, IDC_BUTTON_SAVEGOSD, m_btnSavetoPattren);
}

BOOL C_GOSDCreatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_comBoxBitMode.AddString(_T("1-Bit"));
	m_comBoxBitMode.AddString(_T("2-Bit"));
	m_comBoxBitMode.SetCurSel(m_nComboxIndex);
	UpdateClrGroup(m_nComboxIndex);

	CRect rcWnd;
//	CWnd* pWnd = GetDlgItem(IDC_STATIC_SOURCE);
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TARGET);
	pWnd->GetWindowRect(&rcWnd);
	m_nPreviewW = rcWnd.Width()-4;//picture left/right distance border 2 pixel
	m_nPreviewH = rcWnd.Height()-4;

	//Init ColorButton
	m_btnColor0.SetColor(RGB(0,0,0));
	m_btnColor1.SetColor(RGB(0,0,0));
	m_btnColor2.SetColor(RGB(0,0,0));
	m_btnColor3.SetColor(RGB(0,0,0));
	InitBtnICON();

	UpdateCtrlEnable();
	
	return TRUE;
}

void C_GOSDCreatorDlg::InitBtnICON()
{
	CRect rect;
 	CButton* p_btn1 = (CButton*)GetDlgItem(IDC_BUTTON_EQUAL1);
	p_btn1->GetWindowRect(rect);
	long width1 = rect.right - rect.left;
	long height1 = rect.bottom - rect.top;
	HICON hicon_btn1 = (HICON)LoadImageW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,width1-6,height1-2,LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
	p_btn1->SetIcon(hicon_btn1);
	p_btn1->ShowWindow(FALSE);

	CButton* p_btn2 = (CButton*)GetDlgItem(IDC_BUTTON_EQUAL2);
	p_btn2->GetWindowRect(rect);
	long width2 = rect.right - rect.left;
	long height2 = rect.bottom - rect.top;
	HICON hicon_btn2 = (HICON)LoadImageW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,width2-6,height2-2,LR_DEFAULTCOLOR|LR_CREATEDIBSECTION);
	p_btn2->SetIcon(hicon_btn2);
	p_btn2->ShowWindow(FALSE);

}

BOOL C_GOSDCreatorDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	if(pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONUP)
	{
		MoveColorTest(pMsg);
	}
	if (pMsg->message == WM_MOUSEMOVE)
	{
		RedrawMouse();
		ShowButton(pMsg);
	}

	
	return CDialog::PreTranslateMessage(pMsg);
}

void C_GOSDCreatorDlg::MoveColorTest(MSG* pMsg)
{
	if (!m_bLoadImage || !m_bGenImage)
		return;

	DWORD dwCtrlID[4] = 
	{
		GetDlgItem(IDC_BUTTON_DRAG0)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_DRAG1)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_DRAG2)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_DRAG3)->GetDlgCtrlID()
	};

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		for (int i = 0; i < m_nClrCnt; i++)
		{			
			CWnd* pWnd = NULL;
			pWnd = GetDlgItem(dwCtrlID[i]);
			if (pWnd && pWnd->GetSafeHwnd())
			{
				CRect rcCtrl;
				pWnd->GetWindowRect(&rcCtrl);
				if (rcCtrl.PtInRect(pMsg->pt))
				{
					m_bMoveColor = TRUE;
					m_dwStartIdx = i;
					break;
				}
			}
		}	
	}

	if (pMsg->message == WM_LBUTTONUP)
	{
		if (m_bMoveColor)
		{
			for (int i = 0; i < m_nClrCnt; i++)
			{
				CWnd* pWnd = NULL;
				pWnd = GetDlgItem(dwCtrlID[i]);
				if (pWnd && pWnd->GetSafeHwnd())
				{
					CRect rcCtrl;
					pWnd->GetWindowRect(&rcCtrl);
					if (rcCtrl.PtInRect(pMsg->pt))
					{
						m_dwEndIdx = i;
						ExChangeColorIdx();
						break;
					}
				}
			}
		}
		m_bMoveColor = FALSE;
	}	
}

void C_GOSDCreatorDlg::RedrawMouse()
{
	if (m_bMoveColor)
	 {
	 	HCURSOR hCur  =  LoadCursor( NULL,IDC_SIZEALL ) ;
	 	::SetCursor(hCur);
	 }
	else
	{
		HCURSOR hCur  =  LoadCursor( NULL,IDC_ARROW  ) ;
		::SetCursor(hCur);
	}
}

void C_GOSDCreatorDlg::ShowButton(MSG* pMsg)
{
	DWORD dwCtrlID[4] = 
	{
		GetDlgItem(IDC_BUTTON_EQUAL1)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_EQUAL2)->GetDlgCtrlID()
	};

	for (int i = 0; i < 2; i++)
	{
		if (i==0 && !m_bLoadImage)
			continue;
		if (i==1 && !m_bGenImage)
			continue;

		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(dwCtrlID[i]);
		if (pWnd && pWnd->GetSafeHwnd())
		{
			CRect rcCtrl;
			pWnd->GetWindowRect(&rcCtrl);
			if (rcCtrl.PtInRect(pMsg->pt))
			{
				pWnd->ShowWindow(TRUE);
				break;
			}
			pWnd->ShowWindow(FALSE);
		}
	}	
}



void C_GOSDCreatorDlg::ExChangeColorIdx()
{
	if (m_dwEndIdx == m_dwStartIdx)
		return;

	COLORREF clrTemp1 = m_nColorArrayOld[m_dwEndIdx];
	m_nColorArrayOld[m_dwEndIdx] = m_nColorArrayOld[m_dwStartIdx];
	m_nColorArrayOld[m_dwStartIdx] = clrTemp1;

	COLORREF clrTemp2 = m_nColorArrayNew[m_dwEndIdx];
	m_nColorArrayNew[m_dwEndIdx] = m_nColorArrayNew[m_dwStartIdx];
	m_nColorArrayNew[m_dwStartIdx] = clrTemp2;

	UpdateBtnColor();
	m_bPreViewImgae = FALSE;
	m_bApplytoPara = FALSE;
}

void C_GOSDCreatorDlg::OnPaint()
{
	CDialog::OnPaint();
	UpdateCtrlEnable();

	ReDrawImage();	
}

void C_GOSDCreatorDlg::ReDrawImage()
{
	if (m_bLoadConfig)
		return;

	DrawSrcImage();
	DrawCurrentImage();
}

void C_GOSDCreatorDlg::UpdateCtrlEnable()
{
	if (!m_bLoadImage)
	{
		m_btnGen.EnableWindow(m_bLoadImage);
		m_btnReset.EnableWindow(m_bLoadImage);
		m_btnPreView.EnableWindow(m_bLoadImage);
		m_comBoxBitMode.EnableWindow(TRUE);
		m_btnApplytoPara.EnableWindow(m_bLoadImage);
		m_btnSavetoPattren.EnableWindow(m_bLoadImage);
	}
	else
	{
		m_btnGen.EnableWindow(!m_bGenImage);
		m_btnReset.EnableWindow(m_bGenImage);
		m_btnPreView.EnableWindow(m_bGenImage);
		m_comBoxBitMode.EnableWindow(!m_bGenImage);
		m_btnApplytoPara.EnableWindow(m_bGenImage);
		m_btnSavetoPattren.EnableWindow(m_bGenImage);
	}
	
	CWnd* pWnd = NULL;
	m_btnColor0.EnableWindow(m_bGenImage);
	m_btnColor1.EnableWindow(m_bGenImage);
	BOOL bEnable = m_bGenImage&&(m_nClrCnt==4);
	m_btnColor2.EnableWindow(bEnable);
	m_btnColor3.EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_BUTTON_DRAG0);
	if (pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->EnableWindow(m_bGenImage);
	}
	pWnd = GetDlgItem(IDC_BUTTON_DRAG1);
	if (pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->EnableWindow(m_bGenImage);
	}
	pWnd = GetDlgItem(IDC_BUTTON_DRAG2);
	if (pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->EnableWindow(bEnable);
	}
	pWnd = GetDlgItem(IDC_BUTTON_DRAG3);
	if (pWnd && pWnd->GetSafeHwnd())
	{
		pWnd->EnableWindow(bEnable);
	}
}


void C_GOSDCreatorDlg::DrawSrcImage()
{
	if (!m_bLoadImage)
		return;

	Bitmap imageSource(m_strSourcePath);
	int nW = imageSource.GetWidth();
	int nH = imageSource.GetHeight();
	if (nW <= 0|| nH <= 0 )
	{
		AfxMessageBox(_T("Image corrupted,Please reopen tool."));
		return;
	}

	//Calc Zoom
	int nMax = max(m_nSourceWidth,m_nSourceHeight);
	int nMin = min(m_nSourceWidth,m_nSourceHeight);
	BOOL bNormal = (m_nSourceWidth>m_nSourceHeight)?TRUE:FALSE;

	//Draw image
	CRect rcWnd,rcCtrl;
	this->GetWindowRect(&rcWnd);
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_SOURCE);
	pCtrl->GetWindowRect(&rcCtrl);
	int cx = rcCtrl.left - rcWnd.left;//Static ctrl distance window left.
	int cy = rcCtrl.top  - rcWnd.top; //Static ctrl distance window top.

	Graphics graphics(GetDC()->m_hDC);
	if (bNormal)
		graphics.DrawImage(&imageSource,cx+2,cy+2,m_nPreviewW,m_nPreviewH*nMin/nMax);
	else
		graphics.DrawImage(&imageSource,cx+2,cy+2,m_nPreviewW*nMin/nMax,m_nPreviewH);

}

void C_GOSDCreatorDlg::DrawCurrentImage()
{
	if (!m_bGenImage)
		return;

	Bitmap image(m_strTargetPath);
	int nW = image.GetWidth();
	int nH = image.GetHeight();
	if (nW <= 0|| nH <= 0 )
	{
		AfxMessageBox(_T("Image corrupted,Please reopen tool."));
		return;
	}

	int iAlpha=255;
	Color color1,color2;
	for(int iColumn=0;iColumn<nW;iColumn++)
	{
		for(int iRow=0;iRow<nH;iRow++)
		{
			image.GetPixel(iColumn,iRow,&color1);
			BYTE byR = 0,byG = 0,byB = 0;
			byR = color1.GetR();
			byG = color1.GetG();
			byB = color1.GetB();
			COLORREF colorNew = GetNewColor(color1);

			color2.SetValue(color1.MakeARGB(iAlpha,GetRValue(colorNew),GetGValue(colorNew),GetBValue(colorNew)));
			image.SetPixel(iColumn,iRow,color2);
		}
	}

	int nMax = max(nW,nH);
	int nMin = min(nW,nH);
	BOOL bNormal = (nW>nH)?TRUE:FALSE;

	//Draw image
	CRect rcWnd,rcCtrl;
	this->GetWindowRect(&rcWnd);
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_TARGET);
	pCtrl->GetWindowRect(&rcCtrl);
	int cx = rcCtrl.left - rcWnd.left;//Static ctrl distance window left.
	int cy = rcCtrl.top  - rcWnd.top; //Static ctrl distance window top.
	Graphics graphics(GetDC()->m_hDC);
	if (bNormal)
		graphics.DrawImage(&image,cx+2,cy+2,m_nPreviewW,m_nPreviewH*nMin/nMax);
	else
		graphics.DrawImage(&image,cx+2,cy+2,m_nPreviewW*nMin/nMax,m_nPreviewH);

	m_bPreViewImgae = TRUE;
}

void C_GOSDCreatorDlg::ClearDataAndUI()
{
	//Clear Array
	for (int i = 0; i < DF_END_CNT; i++)
	{
		m_nColorArrayOld[i] = RGB(0,0,0);
		m_nColorArrayNew[i] = RGB(0,0,0);
		m_nColorBackUp[i] = RGB(0,0,0);
	}
	
	//clear UI(buttton)
	m_btnColor0.SetColor(RGB(0,0,0));
	m_btnColor1.SetColor(RGB(0,0,0));
	m_btnColor2.SetColor(RGB(0,0,0));
	m_btnColor3.SetColor(RGB(0,0,0));
}


void C_GOSDCreatorDlg::UpdateClrGroup(int nMode)
{
	if (nMode != 0 && nMode != 1)
		return;

	DWORD CtrlIDArray[] =
	{
		GetDlgItem(IDC_STATIC_COLOR2)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_COLOR2)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_DRAG2)->GetDlgCtrlID(),
	
		GetDlgItem(IDC_STATIC_COLOR3)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_COLOR3)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_DRAG3)->GetDlgCtrlID(),
	};

	BOOL bShow = FALSE;
	if (nMode == 0)
		bShow = FALSE;
	else if (nMode == 1)
		bShow = TRUE;
	for (int i = 0; i < sizeof(CtrlIDArray)/sizeof(CtrlIDArray[0]);i++)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(CtrlIDArray[i]);
		if (pWnd)
		{
			pWnd->ShowWindow(bShow);
		}
	}
}

void C_GOSDCreatorDlg::SetModelMgr(I_GOSDDataMgr* pGOSDMgr)
{
	ASSERT(pGOSDMgr);
	m_pGOSGMgr = pGOSDMgr;
	m_nMaxSize = m_pGOSGMgr->GetItemValue(GOSD_MAXSIZE);
	//InitPrjCfg
	CString strConfigFilePath = _T("");
	CBurningToolGOSDCreatorApp* pGOSDCreatorApp = (CBurningToolGOSDCreatorApp*)AfxGetApp();
	if (!pGOSDCreatorApp->m_strBurPrjPath.IsEmpty())
	{
		strConfigFilePath = pGOSDCreatorApp->m_strBurPrjPath + _T("\\GOSD_Parameters.txt");
		m_pGOSGMgr->LoadGOSDFile(strConfigFilePath);
		m_bLoadConfig = TRUE;
		m_bApplytoPara = TRUE;
		UpdateModeltoUI();

	}
}

void C_GOSDCreatorDlg::SaveUItoMadel()
{
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return;

	SaveBtnColor();//UI->New
	//Image Info
	UpdateData(TRUE);
	m_pGOSGMgr->SetItemValue(GOSD_SIZE_W,m_dwWidth);
	m_pGOSGMgr->SetItemValue(GOSD_SIZE_H,m_dwHeight);
	m_pGOSGMgr->SetItemValue(GOSD_BIT_MODE,m_nComboxIndex);
	UpdateData(FALSE);
	//Center
	if ((m_nSourceWidth != 0)&&(m_nSourceHeight != 0))
	{
		DWORD dwScaleValue = m_pGOSGMgr->GetItemValue(GOSD_SCALE_VALUE);
		DWORD dwResX = m_pGOSGMgr->GetItemValue(GOSD_RES_X);
		DWORD dwResY = m_pGOSGMgr->GetItemValue(GOSD_RES_Y);
		int nStartX = (dwResX - (m_nSourceWidth *(dwScaleValue + 1)))/2 + 1;
		int nStartY = (dwResY - (m_nSourceHeight *(dwScaleValue + 1)))/2 + 1;
		m_pGOSGMgr->SetItemValue(GOSD_START_X,nStartX);
		m_pGOSGMgr->SetItemValue(GOSD_START_Y,nStartY);
	}
	//Color
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0,m_nColorArrayNew[0]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1,m_nColorArrayNew[1]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2,m_nColorArrayNew[2]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3,m_nColorArrayNew[3]);
	SavetoYUV();//RGB->YUV
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_Y,m_nClr0YUV[0]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_U,m_nClr0YUV[1]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_V,m_nClr0YUV[2]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_Y,m_nClr1YUV[0]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_U,m_nClr1YUV[1]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_V,m_nClr1YUV[2]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_Y,m_nClr2YUV[0]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_U,m_nClr2YUV[1]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_V,m_nClr2YUV[2]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_Y,m_nClr3YUV[0]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_U,m_nClr3YUV[1]);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_V,m_nClr3YUV[2]);
}
void C_GOSDCreatorDlg::UpdateModeltoUI()
{
	//Parameter no need Update to Creator 
/*
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return;
	//Image Info
	UpdateData(TRUE);
 	m_dwWidth = m_pGOSGMgr->GetItemValue(GOSD_SIZE_W);
 	m_dwHeight = m_pGOSGMgr->GetItemValue(GOSD_SIZE_H);
	m_nComboxIndex = m_pGOSGMgr->GetItemValue(GOSD_BIT_MODE);
	if (m_nComboxIndex == 0)
		m_nClrCnt = 2;
	else if (m_nComboxIndex == 1)
		m_nClrCnt = 4;
	m_comBoxBitMode.SetCurSel(m_nComboxIndex);
	UpdateData(FALSE);

	//Color
	m_nColorArrayNew[0] = m_pGOSGMgr->GetItemValue(GOSD_COLOR0);
	m_nColorArrayNew[1] = m_pGOSGMgr->GetItemValue(GOSD_COLOR1);
	m_nColorArrayNew[2] = m_pGOSGMgr->GetItemValue(GOSD_COLOR2);
	m_nColorArrayNew[3] = m_pGOSGMgr->GetItemValue(GOSD_COLOR3);
	UpdateBtnColor();
*/
}

void C_GOSDCreatorDlg::SavetoYUV()
{
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return;

	m_pGOSGMgr->GetYUV(m_nColorArrayNew[0],&m_nClr0YUV[0],&m_nClr0YUV[1],&m_nClr0YUV[2]);
	m_pGOSGMgr->GetYUV(m_nColorArrayNew[1],&m_nClr1YUV[0],&m_nClr1YUV[1],&m_nClr1YUV[2]);
	m_pGOSGMgr->GetYUV(m_nColorArrayNew[2],&m_nClr2YUV[0],&m_nClr2YUV[1],&m_nClr2YUV[2]);
	m_pGOSGMgr->GetYUV(m_nColorArrayNew[3],&m_nClr3YUV[0],&m_nClr3YUV[1],&m_nClr3YUV[2]);
}


BEGIN_MESSAGE_MAP(C_GOSDCreatorDlg, CDialog)
	ON_WM_PAINT()
//	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &C_GOSDCreatorDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_GEN, &C_GOSDCreatorDlg::OnBnClickedButtonGen)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &C_GOSDCreatorDlg::OnBnClickedButtonReset)
	ON_CBN_SELCHANGE(IDC_COMBO_BIT_MODE, &C_GOSDCreatorDlg::OnCbnSelchangeComboBitMode)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &C_GOSDCreatorDlg::OnBnClickedButtonPreview)
	ON_BN_CLICKED(IDC_BUTTON_SAVEGOSD, &C_GOSDCreatorDlg::OnBnClickedButtonSavegosd)
	ON_BN_CLICKED(IDC_BUTTON_APPLYGOSD, &C_GOSDCreatorDlg::OnBnClickedButtonApplygosd)
	ON_BN_CLICKED(IDC_BUTTON_COLOR0, &C_GOSDCreatorDlg::OnBnClickedButtonColor0)
	ON_BN_CLICKED(IDC_BUTTON_COLOR1, &C_GOSDCreatorDlg::OnBnClickedButtonColor1)
	ON_BN_CLICKED(IDC_BUTTON_COLOR2, &C_GOSDCreatorDlg::OnBnClickedButtonColor2)
	ON_BN_CLICKED(IDC_BUTTON_COLOR3, &C_GOSDCreatorDlg::OnBnClickedButtonColor3)
	ON_BN_CLICKED(IDC_BUTTON_EQUAL1, &C_GOSDCreatorDlg::OnBnClickedButtonEqual1)
	ON_BN_CLICKED(IDC_BUTTON_EQUAL2, &C_GOSDCreatorDlg::OnBnClickedButtonEqual2)
END_MESSAGE_MAP()


// C_GOSDCreatorDlg message handlers

void C_GOSDCreatorDlg::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CString strPath;
	CString strFileFilter = _T("");
	UINT nOpenFlags = OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST;
	CFileDialog fileDlg( TRUE, NULL, NULL, nOpenFlags, strFileFilter, this);
	if (fileDlg.DoModal() == IDOK)
	{
		strPath = fileDlg.GetPathName();

		Bitmap imageSource(strPath);//Load Image
		int nWidth = imageSource.GetWidth();
		int nHeight = imageSource.GetHeight();

		//Check
 		if(!CheckWHSize(nWidth,nHeight))
			return;

		int nMode = 1;
		if (!CheckGOSDSize(nWidth,nHeight,nMode,m_nMaxSize))
		{
			nMode = 0;
			if (!CheckGOSDSize(nWidth,nHeight,nMode,m_nMaxSize))
			{
				CString strMsg = _T("");
				strMsg.Format(_T("GOSD size is over %d KByte."),m_nMaxSize);
				AfxMessageBox(strMsg);
				return;
			}
		}
		m_nComboxIndex = min(nMode,m_nComboxIndex);
		m_nClrCnt = (m_nComboxIndex==0)?2:4;
		m_comBoxBitMode.SetCurSel(m_nComboxIndex);
		UpdateClrGroup(m_nComboxIndex);	

		m_strSourcePath = _T("");
		m_strTargetPath = _T("");

		m_strSourcePath = strPath;
		m_nSourceWidth = nWidth;
		m_nSourceHeight = nHeight;
		UpdateImageInfo();

		DrawSrcImage();
		m_bLoadImage = TRUE;
		m_bLoadConfig = FALSE;
		m_bGenImage = FALSE;
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
		ClearDataAndUI();
		UpdateCtrlEnable();
		
		Invalidate();
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonGen()
{
	// TODO: Add your control notification handler code here
	CString ImagePath = m_strSourcePath;

	if(!m_bLoadImage)
	{
		AfxMessageBox(_T("Please Load Image First!"));
		return;
	}

 	else
	{
#if 0
		//Clear 
		memset(m_nColorCnt,0,sizeof(int)*DF_PALLET_MAT);
		memset(m_nColorArrayOld,0,sizeof(COLORREF)*DF_END_CNT);
		memset(m_nColorArrayNew,0,sizeof(COLORREF)*DF_END_CNT);
		m_mapCntOrder.clear();


		Bitmap image(ImagePath);//Load Image
		int nWidth = image.GetWidth();
		int nHeight = image.GetHeight();
		if (nWidth <= 0|| nHeight <= 0 )
		{
			AfxMessageBox(_T("Image corrupted,Please reopen tool."));
			return;
		}
		//Calculate Pixel
		Color color;
		for(int iColumn=0;iColumn<nWidth;iColumn++)
		{
			for(int iRow=0;iRow<nHeight;iRow++)
			{
				image.GetPixel(iColumn,iRow,&color);
				int nIndex = GetNearIdx(color.GetRed(),color.GetGreen(),color.GetBlue());
				m_nColorCnt[nIndex] += 1; 
			}
		}
		SaveFirstMaxCnt();//Order in Map
		GetFinalPallet(m_nMode);

		//1.Save Image2 to File  2.Show Image2
		Bitmap image2(ImagePath);
		int nW = image2.GetWidth();
		int nH = image2.GetHeight();
		if (nW <= 0|| nH <= 0 )
		{
			AfxMessageBox(_T("Image corrupted,Please reopen tool."));
			return;
		}

		Bitmap* pNewImage= new Bitmap(nW,nH);
		Graphics* pGraphics = new Graphics(pNewImage);
		SolidBrush clearBrush(Color::White);
		pGraphics->FillRectangle(&clearBrush,0,0,nW,nH);

		int iAlpha=255;
		Color color1,color2;
		for(int iColumn=0;iColumn<nW;iColumn++)
		{
			for(int iRow=0;iRow<nH;iRow++)
			{
				image2.GetPixel(iColumn,iRow,&color1);
				BYTE byR = 0,byG = 0,byB = 0;
				byR = color1.GetR();
				byG = color1.GetG();
				byB = color1.GetB();
				COLORREF afterColor = GetCovertColor(byR,byG,byB);

				color2.SetValue(color1.MakeARGB(iAlpha,GetRValue(afterColor),GetGValue(afterColor),GetBValue(afterColor)));
				image2.SetPixel(iColumn,iRow,color2);
				pNewImage->SetPixel(iColumn,iRow,color2);
			}
		}

		//Save Image2
		CLSID Clsid;
		GetEncoderClsid(m_strImageType, &Clsid);//get Encoder by Type 

		CString strSavePath = _T("./New.bmp");
		wchar_t* picPathW = strSavePath.AllocSysString();
		wchar_t picPathW1[100];
		wcscpy(picPathW1,picPathW);
		pNewImage->Save(picPathW1, &Clsid);
		delete pNewImage;
		delete pGraphics;
		pGraphics = NULL;
		pNewImage = NULL;

		m_strTargetPath = strSavePath;
		m_bGenImage = TRUE;
		BackUpColor();
		UpdateBtnColor();
		UpdateCtrlEnable();

#else
		//OpenCV
		TCHAR pFullPath[MAX_PATH] ={0}; 
		GetCurrentDirectory( MAX_PATH,pFullPath); 
		CString strInitGenFilePath = pFullPath;
		if (strInitGenFilePath.GetLength() > 0)
			strInitGenFilePath += _T("\\InitGen.bmp");
		else
			strInitGenFilePath = _T("C:/InitGen.bmp");
		m_strTargetPath = strInitGenFilePath;

		BOOL bConvert = m_pGOSGMgr->ConvertImage(m_nClrCnt,m_strSourcePath,m_strTargetPath);
		if (!bConvert)
		{
			AfxMessageBox(_T("Gen image failed,please try again."));
			return;
		}
		//Init color Array
		Bitmap image(m_strTargetPath);//Load Image
		int nWidth = image.GetWidth();
		int nHeight = image.GetHeight();
		if (nWidth <= 0|| nHeight <= 0 )
		{
			AfxMessageBox(_T("Image corrupted,Please reopen tool."));
			return;
		}

		std::set<COLORREF> setTemp; 
		Color color;
		for(int iColumn=0;iColumn<nWidth;iColumn++)
		{
			for(int iRow=0;iRow<nHeight;iRow++)
			{
				image.GetPixel(iColumn,iRow,&color);
				BYTE byR = color.GetR();
				BYTE byG = color.GetG();
				BYTE byB = color.GetB();

				COLORREF clrTemp = RGB(byR,byG,byB); 
				if(setTemp.find(clrTemp) == setTemp.end())
				{
					setTemp.insert(clrTemp);
				}
			}
		}

		memset(m_nColorArrayOld,0,sizeof(COLORREF)*DF_END_CNT);
		memset(m_nColorArrayNew,0,sizeof(COLORREF)*DF_END_CNT);
		memset(m_nColorBackUp,0,sizeof(COLORREF)*DF_END_CNT);
		std::set<COLORREF>::iterator it = setTemp.begin();
		for (int i = 0; it != setTemp.end(); ++it,i++)
		{
			m_nColorArrayOld[i] = (*it);
			m_nColorArrayNew[i] = (*it);
			m_nColorBackUp[i] = (*it);
		}

		m_bGenImage = TRUE;
		BackUpColor();
		UpdateBtnColor();
		UpdateCtrlEnable();

		DrawCurrentImage();
#endif
 	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonReset()
{
	// TODO: Add your control notification handler code here 
	ReSetColor();//Back->New
	UpdateBtnColor();
	DrawCurrentImage();
	m_bResetImage = TRUE;
}

void C_GOSDCreatorDlg::OnBnClickedButtonPreview()
{
	// TODO: Add your control notification handler code here
	SaveBtnColor();
	DrawCurrentImage();
}




int C_GOSDCreatorDlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}


void C_GOSDCreatorDlg::OnBnClickedButtonSavegosd()
{
	if (!m_bPreViewImgae)
	{
		if (AfxMessageBox(_T("You no preview.Continue to SavePattern?"),MB_OKCANCEL) != IDOK)
			return;
	}

	TCHAR pFullPath[MAX_PATH] ={0}; 
	GetCurrentDirectory( MAX_PATH,pFullPath); 
	CString strEndGenFilePath = pFullPath;
	if (strEndGenFilePath.GetLength() > 0)
		strEndGenFilePath += _T("\\EndGen.bmp");
	else
		strEndGenFilePath = _T("C:/EndGen.bmp");

	if (!SavePreview2File(strEndGenFilePath))
		return;
	

	CString strBinFilePath = _T("");
	CBurningToolGOSDCreatorApp* pGOSDCreatorApp = (CBurningToolGOSDCreatorApp*)AfxGetApp();
	if (!pGOSDCreatorApp->m_strBurPrjPath.IsEmpty())
	{
		CString strBinFileName = pGOSDCreatorApp->m_strChipNamePrefix + _T("_GOSD.bin");
		strBinFilePath = pGOSDCreatorApp->m_strBurPrjPath + _T("\\") + strBinFileName;
	}
	else
	{
		CString strFolder = _T("");
		strFolder = SelFolderPath();
		if (strFolder.GetLength() > 0)
		{
			strBinFilePath = strFolder + _T("\\Image_GOSD.bin");
		}
	}

	if (strBinFilePath.GetLength() > 0)
	{
		//Gen bin File
		if (SaveBinFile(strEndGenFilePath,strBinFilePath))
			AfxMessageBox(_T("Save GOSD Pattern Success."));
		else
			AfxMessageBox(_T("Save GOSD Pattern Failed."));
	}
}
BOOL C_GOSDCreatorDlg::SavePreview2File(CString strFilePath)
{
	Bitmap image(m_strTargetPath);
	int nW = image.GetWidth();
	int nH = image.GetHeight();
	if (nW <= 0|| nH <= 0 )
	{
		AfxMessageBox(_T("Image corrupted,Please reopen tool."));
		return FALSE;
	}

	//Save preview to file
	CLSID Clsid;
	if(GetEncoderClsid(m_strImageType, &Clsid) == -1)//get Encoder by Type 
	{
		AfxMessageBox(_T("Can't save final file,Please try again or reload image."));
		return FALSE;
	}

	Bitmap* pFinalImage= new Bitmap(nW,nH);
	Graphics* pGraphics = new Graphics(pFinalImage);
	SolidBrush clearBrush(Color::White);
	pGraphics->FillRectangle(&clearBrush,0,0,nW,nH);
	int iAlpha=255;
	Color color1,color2;
	for(int iColumn=0;iColumn<nW;iColumn++)
	{
		for(int iRow=0;iRow<nH;iRow++)
		{
			image.GetPixel(iColumn,iRow,&color1);
			BYTE byR = 0,byG = 0,byB = 0;
			byR = color1.GetR();
			byG = color1.GetG();
			byB = color1.GetB();
			COLORREF colorNew = GetNewColor(color1);

			color2.SetValue(color1.MakeARGB(iAlpha,GetRValue(colorNew),GetGValue(colorNew),GetBValue(colorNew)));
			pFinalImage->SetPixel(iColumn,iRow,color2);
		}
	}
	TCHAR* picPathW = strFilePath.AllocSysString();
	TCHAR picPathW1[MAX_PATH];
	wcscpy_s(picPathW1,picPathW);
	pFinalImage->Save(picPathW1, &Clsid);

	delete pFinalImage;
	delete pGraphics;
	pGraphics = NULL;
	pFinalImage = NULL;

	return TRUE;
}
CString C_GOSDCreatorDlg::SelFolderPath()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	CString         strFolderPath = TEXT("");  

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("Please Chose a Folder.");  
	sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;  
	sInfo.lpfn     = NULL;  

	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{   
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}  

	return strFolderPath;  
}

BOOL C_GOSDCreatorDlg::SaveBinFile(CString strImagePath,CString strBinFilePath)
{
	//1.open file
	CString strErrorMsg;
	FILE* pFile = _tfopen(strBinFilePath,_T("wb"));
	if (pFile == NULL)
	{
		strErrorMsg = _T("Error: open the file failed, please check !");
		return FALSE;
	}

	//2.Write
	Bitmap imageEnd(strImagePath);
	int nEndW = imageEnd.GetWidth();
	int nEndH = imageEnd.GetHeight();
	if (nEndW <= 0|| nEndH <= 0 )
	{
		AfxMessageBox(_T("Image corrupted,Please reopen tool."));
		return FALSE;
	}

	int nArraySz = (m_nClrCnt==2)?8:4;
	int nOffset = (m_nClrCnt==2)?1:2;
	BYTE* pIdxArray = new BYTE[nArraySz];//0/1/2/3
	memset(pIdxArray,0,sizeof(pIdxArray));

	int nIdx = 0;
	int nPixelCnt = 0;
	int nWriteCnt = 0;
	Color color;

	for(int i=0;i<nEndH;i++)
	{
		for(int j=0;j<nEndW;j++)
		{
			imageEnd.GetPixel(j,i,&color);
			int nIdxValue = GetColorIndex(m_nClrCnt,color);
			pIdxArray[nIdx++] = nIdxValue;

			nPixelCnt = i*nEndW+j;
			if ((nPixelCnt+1)%nArraySz == 0)//Write a BYTE to file
			{
				BYTE byValue = 0;
				for (int k = 0; k < nArraySz; k++)
				{
					byValue |= (pIdxArray[k]<<(k*nOffset));
				}

				nWriteCnt += fwrite(&byValue,1,1,pFile);
				memset(pIdxArray,0,sizeof(pIdxArray));
				nIdx = 0;
			}
		}
	}
	delete[] pIdxArray;

	// 	//Debug Test
	// 	if ((nPixelCnt+1)%nArraySz)
	// 		TRACE0("Picture Size not standard.\n");
	// 	else
	// 		TRACE0("Picture Size is standard.\n");
	// 	if (nWriteCnt == (nPixelCnt+1)/nArraySz)
	// 		AfxMessageBox(_T("Gen Bin File Success."));
	// 	else
	// 		AfxMessageBox(_T("Gen Bin File Failed."));

	//3.close file
	if (pFile != NULL)
	{
		fclose(pFile);
	}
	
	return TRUE;
}

void C_GOSDCreatorDlg::OnBnClickedButtonApplygosd()
{
	// TODO: Add your control notification handler code here
	if (!m_bLoadImage || !m_bGenImage)
		return;

	if (!m_bPreViewImgae)
	{
		if (AfxMessageBox(_T("Color Index has been changed without previewing.Continue to apply parameters?"),MB_OKCANCEL) != IDOK)
 			return;
	}
	
	SaveUItoMadel();
	CWnd* pParent = GetParent();
	if (pParent != NULL )
	{
		pParent->SendMessage(WM_USER_UPDATE_PARAMETER,0,0);
		m_bApplytoPara = TRUE;
		AfxMessageBox(_T("Apply to Parameters Success."));
	}
	else
	{
		m_bApplytoPara = FALSE;
		AfxMessageBox(_T("Apply to Parameters Failed."));
	}

}

void C_GOSDCreatorDlg::OnCbnSelchangeComboBitMode()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_comBoxBitMode.GetCurSel();
	if (m_nComboxIndex == nIndex)
		return;

	int nClrCnt = (nIndex == 0)?2:4;
	if (!m_bLoadImage)
	{
		m_nComboxIndex = nIndex;
		m_nClrCnt = nClrCnt;
		UpdateClrGroup(m_nComboxIndex);
		return;
	}

	//ReCalc GOSD Size when loadImage yet 
	CString strMsg = _T("");
	if (!CheckGOSDSize(m_nSourceWidth,m_nSourceHeight,nIndex,m_nMaxSize))
	{
		strMsg.Format(_T("GOSD size is over %d KByte."),m_nMaxSize);
		AfxMessageBox(strMsg);
		m_comBoxBitMode.SetCurSel(m_nComboxIndex);
		return;
	}
	else
	{
		m_nComboxIndex = nIndex;
		m_nClrCnt = nClrCnt;
		m_comBoxBitMode.SetCurSel(m_nComboxIndex);
		UpdateClrGroup(m_nComboxIndex);
		UpdateImageInfo();
		UpdateCtrlEnable();
		UpdateBtnColor();

		m_bGenImage = FALSE;
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
	}
}



void C_GOSDCreatorDlg::UpdateImageInfo()
{
	UpdateData(TRUE);
	m_dwWidth = m_nSourceWidth;
	m_dwHeight = m_nSourceHeight;
	m_fGOSDSize = (float)m_nSourceWidth*m_nSourceHeight*(m_nComboxIndex+1)/8/1024;
	m_strGOSDSize.Format(_T("%.2f KByte"),m_fGOSDSize);
	UpdateData(FALSE);
}

BOOL C_GOSDCreatorDlg::CheckWHSize(int nWidth,int nHeight)
{
	return TRUE;
	CString strMsg1 = _T("File is invalid,Please try again.");
	CString strMsg2 = _T("Image width must be multiple of 64.");
	CString strMsg3 = _T("Image height must be multiple of 64.");
	CString strMsg4 = _T("Image width or height out of range,Please redelect image.");
	if (nWidth <= 0 || nHeight <= 0)
	{
		AfxMessageBox(strMsg1);
		return FALSE;
	}

	int nMaxWidth = m_pGOSGMgr->GetItemValue(GOSD_PIC_MAX_W);
	int nMaxHeight = m_pGOSGMgr->GetItemValue(GOSD_PIC_MAX_H);
	if (nWidth > nMaxWidth || nHeight > nMaxHeight)
	{
		AfxMessageBox(strMsg4);
		return FALSE;
	}

	if (nWidth%64 != 0)
	{
		AfxMessageBox(strMsg2);
		return FALSE;
	}

// 	if (nHeight%64 != 0)
// 	{
// 		AfxMessageBox(strMsg3);
// 		return FALSE;
// 	}

	return TRUE;
}
BOOL C_GOSDCreatorDlg::CheckGOSDSize(int nWidth,int nHeight,int nMode,int nMaxSize)
{
	return TRUE;
	CString strMsg = _T("");
	if (nWidth <= 0 || nHeight <= 0 || (nMode != 0&&nMode != 1))
	{
		strMsg = _T("Width or Height or nMode is Invalid.");
		AfxMessageBox(strMsg);
		return FALSE;
	}

	float fGOSDSize = (float)nWidth*nHeight*(nMode+1)/8/1024;
	float fMaxSize = (float)nMaxSize;
	if (fGOSDSize > fMaxSize)
	{
		return FALSE;
	}

	return TRUE;
}














int  C_GOSDCreatorDlg::GetNearIdx(BYTE byR,BYTE byG,BYTE byB)
{
	int nTemp = INT_MAX;
	int nMin = INT_MAX;
	int nIndex = 0;
	for (int i = 0; i < DF_PALLET_MAT; i++)
	{
		nTemp = abs(GetRValue(m_colorPallet[i]) - byR) + abs(GetGValue(m_colorPallet[i]) - byG) + abs(GetBValue(m_colorPallet[i]) - byB);
		if (nTemp <= nMin)
		{
			nMin = nTemp;
			nIndex = i;
		}
	}

	return nIndex;
}

void C_GOSDCreatorDlg::SaveFirstMaxCnt()
{
	for (int i = 0; i < DF_PALLET_MAT; i++)
	{
		int nIndex = m_nColorCnt[i];//key = ColorCnt,value = ColorIndex
		m_mapCntOrder[nIndex] = i;
	}

	int nMapSize = m_mapCntOrder.size();
	int nFirstCnt = min(nMapSize,DF_FIRST_CNT);
	std::map<int,int>::iterator it = m_mapCntOrder.end();
	--it;
	m_vecFirstColor.clear();
	for (int j = 0; j < nFirstCnt;j++,--it)//
	{
		int nIndex = (int)(it->second);
		m_vecFirstColor.push_back(m_colorPallet[nIndex]);
	}
}


void C_GOSDCreatorDlg::GetFinalPallet(int nFinalCnt) //first -> end
{
	int nVecSize = m_vecFirstColor.size();
	if (nVecSize < nFinalCnt)
		nFinalCnt = nVecSize;

	while(nVecSize > nFinalCnt)
	{
		int nIndex1 = 0,nIndex2 = 0;
		for (int i = 0; i < nVecSize;i++)
		{
			for (int j = i+1; j < nVecSize; j++)
			{
				int nMin = INT_MAX;
				int nTemp = INT_MAX;
				COLORREF color1 = m_vecFirstColor[i];
				COLORREF color2 = m_vecFirstColor[j];

				int x1 = GetRValue(color1);
				int y1 = GetGValue(color1);
				int z1 = GetBValue(color1);

				int x2 = GetRValue(color2);
				int y2 = GetGValue(color2);
				int z2 = GetBValue(color2);

				double dbResult = (double)(x1*x1+y1*y1+z1*z1 + x2*x2+y2*y2+z2*z2);
				nTemp = sqrt(dbResult);
				if (nTemp <= nMin)
				{
					nMin = nTemp;
					nIndex1 = i;
					nIndex2 = j;
				}
			}
		}

		BYTE byNewR = (GetRValue(m_vecFirstColor[nIndex1])+GetRValue(m_vecFirstColor[nIndex2]))/2;
		BYTE byNewG = (GetGValue(m_vecFirstColor[nIndex1])+GetGValue(m_vecFirstColor[nIndex2]))/2;
		BYTE byNewB = (GetBValue(m_vecFirstColor[nIndex1])+GetBValue(m_vecFirstColor[nIndex2]))/2;
		COLORREF newColor = RGB(byNewR,byNewG,byNewB);


		m_vecFirstColor[nIndex1] = UINT_MAX;
		m_vecFirstColor[nIndex2] = UINT_MAX;
		std::vector<COLORREF> vecTemp(m_vecFirstColor);//
		m_vecFirstColor.clear();
		for (int k = 0; k < vecTemp.size();k++)
		{
			COLORREF value = vecTemp[k]; 
			if (value != UINT_MAX)
			{
				m_vecFirstColor.push_back(value);
			}
		}
		m_vecFirstColor.push_back(newColor);

		nVecSize = m_vecFirstColor.size();
	}
	//Init Old/New Array
	memset(m_nColorArrayOld,0,sizeof(COLORREF)*DF_END_CNT);
	memset(m_nColorArrayNew,0,sizeof(COLORREF)*DF_END_CNT);
	for (int i = 0; i < nFinalCnt; i++)
	{
		m_nColorArrayOld[i] = m_vecFirstColor[i]; //
		m_nColorArrayNew[i] = m_nColorArrayOld[i];//copy to m_nColorArray2
	}
}
COLORREF C_GOSDCreatorDlg::GetCovertColor(BYTE byR,BYTE byG,BYTE byB)
{
	int nTemp = INT_MAX;
	int nMin = INT_MAX;
	int nIndex = 0;
	for (int i = 0; i < m_nClrCnt; i++)
	{
		nTemp = abs(GetRValue(m_nColorArrayOld[i]) - byR) + abs(GetGValue(m_nColorArrayOld[i]) - byG) + abs(GetBValue(m_nColorArrayOld[i]) - byB);
		if (nTemp <= nMin)
		{
			nMin = nTemp;
			nIndex = i;
		}
	}

	return m_nColorArrayOld[nIndex];
}


void C_GOSDCreatorDlg::BackUpColor()
{
	for (int i = 0; i < DF_END_CNT; i++)
	{
		m_nColorBackUp[i] = m_nColorArrayOld[i];
	}
}

void C_GOSDCreatorDlg::ReplaceColor()
{
	for (int i = 0; i < DF_END_CNT; i++)
	{
		m_nColorArrayOld[i] = m_nColorArrayNew[i];
	}
}

void C_GOSDCreatorDlg::ReSetColor()
{
	for (int i = 0; i < DF_END_CNT; i++)
	{
		m_nColorArrayNew[i] = m_nColorBackUp[i];
		m_nColorArrayOld[i] = m_nColorBackUp[i];
	}
}


COLORREF C_GOSDCreatorDlg::GetNewColor(Color oldColor)
{
	BYTE byoldR = oldColor.GetR();
	BYTE byoldG = oldColor.GetG();
	BYTE byoldB = oldColor.GetB();

	COLORREF nRetColor = m_nColorArrayNew[0];
	for (int i = 0; i < m_nClrCnt; i++)
	{
		BYTE byR = GetRValue(m_nColorArrayOld[i]);
		BYTE byG = GetGValue(m_nColorArrayOld[i]);
		BYTE byB = GetBValue(m_nColorArrayOld[i]);
		if (byR==byoldR && byG==byoldG && byB==byoldB)
		{
			nRetColor = m_nColorArrayNew[i];
			break;
		}
	}

	return nRetColor;
}

int C_GOSDCreatorDlg::GetColorIndex(int nBitMode, Color color)
{
	int nIndex = -1;
	if (nBitMode != 2 && nBitMode != 4)
		return nIndex;
	
	BYTE byR = color.GetR();
	BYTE byG = color.GetG();
	BYTE byB = color.GetB();
	for (int i = 0; i < nBitMode; i++)
	{
		BYTE byoldR = GetRValue(m_nColorArrayNew[i]);
		BYTE byoldG = GetGValue(m_nColorArrayNew[i]);
		BYTE byoldB = GetBValue(m_nColorArrayNew[i]);
		if (byR==byoldR && byG==byoldG && byB==byoldB)
		{
			nIndex = i;
			break;
		}
	}
	
	return nIndex;
}


void C_GOSDCreatorDlg::SaveBtnColor()
{
	m_nColorArrayNew[0] = m_btnColor0.GetColor();
	m_nColorArrayNew[1] = m_btnColor1.GetColor();
	m_nColorArrayNew[2] = m_btnColor2.GetColor();
	m_nColorArrayNew[3] = m_btnColor3.GetColor();
}
void C_GOSDCreatorDlg::UpdateBtnColor()
{
	BOOL bModeState = FALSE;
	if (m_nClrCnt == 2)
		bModeState = FALSE;
	else if (m_nClrCnt == 4)
		bModeState = TRUE;

	if (bModeState)
	{
		m_btnColor0.SetColor(m_nColorArrayNew[0]);
		m_btnColor1.SetColor(m_nColorArrayNew[1]);
		m_btnColor2.SetColor(m_nColorArrayNew[2]);
		m_btnColor3.SetColor(m_nColorArrayNew[3]);
	}
	else
	{
		m_btnColor0.SetColor(m_nColorArrayNew[0]);
		m_btnColor1.SetColor(m_nColorArrayNew[1]);
		m_btnColor2.SetColor(RGB(0,0,0));
		m_btnColor3.SetColor(RGB(0,0,0));
	}
}


void C_GOSDCreatorDlg::OnBnClickedButtonColor0()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	clrDlg.InitByColor(m_btnColor0.GetColor());
	if (IDOK == clrDlg.DoModal())
	{
		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor0.SetColor(clrValue);
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
		m_bResetImage = FALSE;
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonColor1()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	clrDlg.InitByColor(m_btnColor1.GetColor());
	if (IDOK == clrDlg.DoModal())
	{
		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor1.SetColor(clrValue);
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
		m_bResetImage = FALSE;
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonColor2()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	clrDlg.InitByColor(m_btnColor2.GetColor());
	if (IDOK == clrDlg.DoModal())
	{
		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor2.SetColor(clrValue);
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
		m_bResetImage = FALSE;
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonColor3()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	clrDlg.InitByColor(m_btnColor3.GetColor());
	if (IDOK == clrDlg.DoModal())
	{
		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor3.SetColor(clrValue);
		m_bPreViewImgae = FALSE;
		m_bApplytoPara = FALSE;
		m_bResetImage = FALSE;
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonEqual1()
{
	// TODO: Add your control notification handler code here
	if (m_bLoadImage)
	{
		m_ImageShow.SetImagePath(m_strSourcePath);
		m_ImageShow.DoModal();
	}
}

void C_GOSDCreatorDlg::OnBnClickedButtonEqual2()
{
	// TODO: Add your control notification handler code here
	if (m_bLoadImage&&m_bGenImage)
	{
		TCHAR pFileName[MAX_PATH] ={0}; 
		GetCurrentDirectory( MAX_PATH,pFileName); 
		CString strNewGenPath = pFileName;
		if (strNewGenPath.GetLength() > 0)
			strNewGenPath += _T("\\NewGen.bmp");
		else
			strNewGenPath = _T("C:/NewGen.bmp");

		if(!SavePreview2File(strNewGenPath))
			return;

		m_ImageShow.SetImagePath(strNewGenPath);
		m_ImageShow.DoModal();
	}
}
