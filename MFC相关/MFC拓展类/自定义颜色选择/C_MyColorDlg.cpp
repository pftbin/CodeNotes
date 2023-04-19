// C_MyColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "C_MyColorDlg.h"


// C_MyColorDlg dialog

IMPLEMENT_DYNAMIC(C_MyColorDlg, CDialog)

C_MyColorDlg::C_MyColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C_MyColorDlg::IDD, pParent)
	, m_nR(0)
	, m_nG(0)
	, m_nB(0)
	, m_nY(0)
	, m_nU(0)
	, m_nV(0)
{
	m_color = RGB(0,255,0);
}

C_MyColorDlg::~C_MyColorDlg()
{
}

void C_MyColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLOR_PALETTE, m_wndColorPalette);
	DDX_Control(pDX, IDC_BUTTON_NOWCLR, m_btnCurClr);
	DDX_Control(pDX, IDC_LUM, m_wndLum);
	DDX_Text(pDX, IDC_EDIT_R, m_nR);
	DDX_Text(pDX, IDC_EDIT_G, m_nG);
	DDX_Text(pDX, IDC_EDIT_B, m_nB);
	DDX_Text(pDX, IDC_EDIT_Y, m_nY);
	DDX_Text(pDX, IDC_EDIT_U, m_nU);
	DDX_Text(pDX, IDC_EDIT_V, m_nV);
}

BOOL C_MyColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CClientDC dc(this);
	int nColors = 256; // Use 256 first entries
	UINT nSize = sizeof(LOGPALETTE) +(sizeof(PALETTEENTRY) * nColors);
	LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

	pLP->palVersion = 0x300;
	pLP->palNumEntries = (USHORT) nColors;

	::GetSystemPaletteEntries(dc.GetSafeHdc(), 0, nColors, pLP->palPalEntry);

	m_palSys.CreatePalette(pLP);

	delete[] pLP;

	m_wndColorPalette.SetType(CMFCColorPickerCtrl::PICKER);
	m_wndColorPalette.SetPalette(&m_palSys);
	m_wndLum.SetType(CMFCColorPickerCtrl::LUMINANCE);
	m_wndLum.SetPalette(&m_palSys);

	m_wndLum.SetColor(m_color);
	m_wndColorPalette.SetColor(m_color);

	double dS1 = m_wndColorPalette.GetSaturation();
	double dS2 = m_wndLum.GetSaturation();
	double dFlag = 0.000000000001;
	if( (1.0 - dS1) < dFlag || (1.0 - dS2) < dFlag		//can't == 1
	  ||(dS1 - 0.0) < dFlag || (dS2 - 0.0) < dFlag )	//can't == 0
	{
		double dH = 0.5,dL = 0.5,dS = 0.5;
		m_wndColorPalette.SetHLS(dH,dL,dS);
		m_wndLum.SetHLS(dH,dL,dS);
	}

	UpdateData(FALSE);

	return TRUE;
}

void C_MyColorDlg::InitByColor(COLORREF clr)
{
	m_color = clr;
	m_nR = GetRValue(m_color);
	m_nG = GetGValue(m_color);
	m_nB = GetBValue(m_color);
	m_btnCurClr.SetColor(m_color);
	int nY = 0,nU = 0,nV = 0;
	GetYUV(m_color,&nY,&nU,&nV);
	m_nY = nY;
	m_nU = nU;
	m_nV = nV;
}

COLORREF C_MyColorDlg::GetColor()
{
	return m_color;
}

void C_MyColorDlg::InitByYUV(int nY,int nU,int nV)
{
	m_nY = nY;
	m_nU = nU;
	m_nV = nV;
	m_color = GetClrByYUV(nY,nU,nV);
	m_nR = GetRValue(m_color);
	m_nG = GetGValue(m_color);
	m_nB = GetBValue(m_color);
	m_btnCurClr.SetColor(m_color);
}
void C_MyColorDlg::GetYUV(int* nY,int* nU,int* nV)
{
	*nY = m_nY;
	*nU = m_nU;
	*nV = m_nV;
}


BEGIN_MESSAGE_MAP(C_MyColorDlg, CDialog)
	ON_BN_CLICKED(IDC_COLOR_PALETTE, &C_MyColorDlg::OnBnClickedColorPalette)
	ON_BN_CLICKED(IDC_LUM, &C_MyColorDlg::OnBnClickedLum)
	ON_EN_CHANGE(IDC_EDIT_R, &C_MyColorDlg::OnEnChangeEditR)
	ON_EN_CHANGE(IDC_EDIT_G, &C_MyColorDlg::OnEnChangeEditG)
	ON_EN_CHANGE(IDC_EDIT_B, &C_MyColorDlg::OnEnChangeEditB)
	ON_EN_CHANGE(IDC_EDIT_Y, &C_MyColorDlg::OnEnChangeEditY)
	ON_EN_CHANGE(IDC_EDIT_U, &C_MyColorDlg::OnEnChangeEditU)
	ON_EN_CHANGE(IDC_EDIT_V, &C_MyColorDlg::OnEnChangeEditV)
END_MESSAGE_MAP()

void C_MyColorDlg::UpdateUI()
{
	UpdateCtrlColor();
	UpdateEditRGB();
	UpdateEditYUV();
}

void C_MyColorDlg::UpdateCtrlColor()
{
	m_btnCurClr.SetColor(m_color);
}

void C_MyColorDlg::UpdateEditRGB()
{
	m_nR = GetRValue(m_color);
	m_nG = GetGValue(m_color);
	m_nB = GetBValue(m_color);
	UpdateData(FALSE);
}
void C_MyColorDlg::UpdateEditYUV()
{
	GetYUV(m_color,&m_nY,&m_nU,&m_nV);
	UpdateData(FALSE);
}

void C_MyColorDlg::GetYUV(COLORREF clr,int* nY,int* nU,int* nV)
{
	int R = GetRValue(clr);
	int G = GetGValue(clr);
	int B = GetBValue(clr);

	*nY = R *  .299000 + G *  .587000 + B *  .114000;
	*nU = R * -.168736 + G * -.331264 + B *  .500000 + 128;
	*nV = R *  .500000 + G * -.418688 + B * -.081312 + 128;
}

COLORREF C_MyColorDlg::GetClrByYUV(int nY,int nU,int nV)
{
	int R = 0,G = 0,B = 0;
	R = nY + 1.4075 * (nV - 128);
	G = nY - 0.3455 * (nU - 128) - (0.7169 * (nV - 128));
	B = nY + 1.7790 * (nU - 128);

	return RGB(R,G,B);
}
// C_MyColorDlg message handlers
void C_MyColorDlg::OnBnClickedColorPalette()
{
	// TODO: Add your control notification handler code here
	COLORREF color = m_wndColorPalette.GetColor();
	m_wndLum.SetColor(color);

	m_color = color;
	UpdateUI();
}

void C_MyColorDlg::OnBnClickedLum()
{
	// TODO: Add your control notification handler code here
	COLORREF color = m_wndLum.GetColor();
	m_wndColorPalette.SetLuminance(m_wndLum.GetLuminance());

	m_color = color;
	UpdateUI();
}
void C_MyColorDlg::OnEnChangeEditR()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nR > 255)
		m_nR = m_nR/10;
	
	COLORREF newClr = RGB(m_nR,m_nG,m_nB);
	m_color = newClr;
	UpdateEditYUV();
	UpdateCtrlColor();
}

void C_MyColorDlg::OnEnChangeEditG()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nG > 255)
		m_nG = m_nG/10;
	COLORREF newClr = RGB(m_nR,m_nG,m_nB);
	m_color = newClr;
	UpdateEditYUV();
	UpdateCtrlColor();
}

void C_MyColorDlg::OnEnChangeEditB()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nB > 255)
		m_nB = m_nB/10;
	COLORREF newClr = RGB(m_nR,m_nG,m_nB);
	m_color = newClr;
	UpdateEditYUV();
	UpdateCtrlColor();
}

void C_MyColorDlg::OnEnChangeEditY()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nY > 255)
		m_nY = m_nY/10;
	if (m_nY < 0)
		m_nY = 0;
	COLORREF newClr = GetClrByYUV(m_nY,m_nU,m_nV);
	m_color = newClr;
	UpdateEditRGB();
	UpdateCtrlColor();
	UpdateData(FALSE);
}

void C_MyColorDlg::OnEnChangeEditU()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nU > 255)
		m_nU = m_nU/10;
// 	if (m_nU < 128)
// 		m_nU = 128;
	COLORREF newClr = GetClrByYUV(m_nY,m_nU,m_nV);
	m_color = newClr;
	UpdateEditRGB();
	UpdateCtrlColor();
	UpdateData(FALSE);
}

void C_MyColorDlg::OnEnChangeEditV()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	while(m_nV > 255)
		m_nV = m_nV/10;
// 	if (m_nV < 128)
// 		m_nV = 128;
	COLORREF newClr = GetClrByYUV(m_nY,m_nU,m_nV);
	m_color = newClr;
	UpdateEditRGB();
	UpdateCtrlColor();
	UpdateData(FALSE);
}
