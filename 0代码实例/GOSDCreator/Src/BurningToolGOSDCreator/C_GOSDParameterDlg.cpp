// C_GOSDParameter.cpp : implementation file
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "C_GOSDParameterDlg.h"

#include "C_MyColorDlg.h"
#define WM_UPDATE_GOSD_TO_CUSTOM		(WM_USER + 115)
// C_GOSDParameter dialog

IMPLEMENT_DYNAMIC(C_GOSDParameterDlg, CDialog)

C_GOSDParameterDlg::C_GOSDParameterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C_GOSDParameterDlg::IDD, pParent)
	, m_pGOSGMgr(NULL)
{
	for (int i = 0; i < 16; i++)
	{
		if (i ==5 || i == 6)
			m_nCheckEnable[i] = 1;
		else
			m_nCheckEnable[i] = 0;
	}
}

C_GOSDParameterDlg::~C_GOSDParameterDlg()
{
}

void C_GOSDParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ENABLE, m_comBoxEnable);
	DDX_Control(pDX, IDC_COMBO_TRIGMODE, m_comBoxTrigMode);
	DDX_Control(pDX, IDC_COMBO_BITMODE2, m_comBoxBitMode);
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comBoxScale);
	DDX_Control(pDX, IDC_COMBO_BK_TIE_YUV, m_comBoxBKTieYUV);
	DDX_Control(pDX, IDC_CHECK0, m_Ckeck0);
	DDX_Control(pDX, IDC_CHECK1, m_Ckeck1);
	DDX_Control(pDX, IDC_CHECK2, m_Ckeck2);
	DDX_Control(pDX, IDC_CHECK3, m_Ckeck3);
	DDX_Control(pDX, IDC_CHECK4, m_Ckeck4);
	DDX_Control(pDX, IDC_CHECK5, m_Ckeck5);
	DDX_Control(pDX, IDC_CHECK6, m_Ckeck6);
	DDX_Control(pDX, IDC_CHECK7, m_Ckeck7);
	DDX_Control(pDX, IDC_CHECK8, m_Ckeck8);
	DDX_Control(pDX, IDC_CHECK9, m_Ckeck9);
	DDX_Control(pDX, IDC_CHECK10, m_Ckeck10);
	DDX_Control(pDX, IDC_CHECK11, m_Ckeck11);
	DDX_Control(pDX, IDC_CHECK12, m_Ckeck12);
	DDX_Control(pDX, IDC_CHECK13, m_Ckeck13);
	DDX_Control(pDX, IDC_CHECK14, m_Ckeck14);
	DDX_Control(pDX, IDC_CHECK15, m_Ckeck15);
	
	DDX_Text(pDX, IDC_EDIT_RES_X, m_strResX);
	DDX_Text(pDX, IDC_EDIT_RES_Y, m_strResY);
	DDX_Text(pDX, IDC_EDIT_STARTX,m_strStartX);
	DDX_Text(pDX, IDC_EDIT_STARTY,m_strStartY);
	DDX_Text(pDX, IDC_EDIT_SIZEW, m_strSizeW);
	DDX_Text(pDX, IDC_EDIT_SIZEH, m_strSizeH);
																
	DDX_Text(pDX, IDC_EDIT_Y0, m_strY0);
	DDX_Text(pDX, IDC_EDIT_U0, m_strU0);
	DDX_Text(pDX, IDC_EDIT_V0, m_strV0);
	DDX_Text(pDX, IDC_EDIT_Y1, m_strY1);
	DDX_Text(pDX, IDC_EDIT_U1, m_strU1);
	DDX_Text(pDX, IDC_EDIT_V1, m_strV1);
	DDX_Text(pDX, IDC_EDIT_Y2, m_strY2);
	DDX_Text(pDX, IDC_EDIT_U2, m_strU2);
	DDX_Text(pDX, IDC_EDIT_V2, m_strV2);
	DDX_Text(pDX, IDC_EDIT_Y3, m_strY3);
	DDX_Text(pDX, IDC_EDIT_U3, m_strU3);
	DDX_Text(pDX, IDC_EDIT_V3, m_strV3);
	DDX_Control(pDX, IDC_BUTTON_SET_CENTER, m_btnSetCenter);
	DDX_Control(pDX, IDC_BUTTON_CLR0, m_btnColor0);
	DDX_Control(pDX, IDC_BUTTON_CLR1, m_btnColor1);
	DDX_Control(pDX, IDC_BUTTON_CLR2, m_btnColor2);
	DDX_Control(pDX, IDC_BUTTON_CLR3, m_btnColor3);
}

BOOL C_GOSDParameterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SaveCheckCtrlID();
	InitCheckEnable();
	AddComBoxItem();
	InitCtrlIDVector();
	InitDescMap();

	int nMode = m_comBoxBitMode.GetCurSel();
	UpdateClrGroup(nMode);

	return TRUE;
}

BOOL C_GOSDParameterDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	if( pMsg->message == WM_CHAR )
	{
		if(pMsg->wParam == VK_BACK || pMsg->wParam == VK_DELETE)
		{
			//do nothing
		}
		else if((pMsg->wParam >= 48 && pMsg->wParam <= 57)		//0-9
			 || (pMsg->wParam >= 65 && pMsg->wParam <= 70)		//A-F
			 || (pMsg->wParam >= 97 && pMsg->wParam <= 102))	//a-f 
		{
			//do nothing
		}
		else
		{
			return TRUE;
		}
	}
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		ShowDescByPoint(pMsg->pt);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void C_GOSDParameterDlg::SetModelMgr(I_GOSDDataMgr* pGOSDMgr)
{
	ASSERT(pGOSDMgr);
	m_pGOSGMgr = pGOSDMgr;
}

void C_GOSDParameterDlg::SaveUItoMadel()
{
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return;
	//ComBox
	DWORD dwValue = 0;
	dwValue = m_comBoxEnable.GetCurSel();
	m_pGOSGMgr->SetItemValue(GOSD_ENABLE,dwValue);
	dwValue = m_comBoxTrigMode.GetCurSel();
	m_pGOSGMgr->SetItemValue(GOSD_TRIG_MODE,dwValue);
	dwValue = m_comBoxBitMode.GetCurSel();
	m_pGOSGMgr->SetItemValue(GOSD_BIT_MODE,dwValue);
	dwValue = m_comBoxScale.GetCurSel();
	m_pGOSGMgr->SetItemValue(GOSD_SCALE_VALUE,dwValue);
	dwValue = m_comBoxBKTieYUV.GetCurSel();
	m_pGOSGMgr->SetItemValue(GOSD_BK_TIE_YUV,dwValue);

	//Edit
	UpdateData(TRUE);
	dwValue = Str2Int(m_strResX,10);
	m_pGOSGMgr->SetItemValue(GOSD_RES_X,dwValue);
	dwValue = Str2Int(m_strResY,10);
	m_pGOSGMgr->SetItemValue(GOSD_RES_Y,dwValue);
	dwValue = Str2Int(m_strStartX,10);
	m_pGOSGMgr->SetItemValue(GOSD_START_X,dwValue);
	dwValue = Str2Int(m_strStartY,10);
	m_pGOSGMgr->SetItemValue(GOSD_START_Y,dwValue);
	dwValue = Str2Int(m_strSizeW,10);
	m_pGOSGMgr->SetItemValue(GOSD_SIZE_W,dwValue);
	dwValue = Str2Int(m_strSizeH,10);
	m_pGOSGMgr->SetItemValue(GOSD_SIZE_H,dwValue);

	dwValue = Str2Int(m_strY0,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_Y,dwValue);
	dwValue = Str2Int(m_strU0,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_U,dwValue);
	dwValue = Str2Int(m_strV0,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0_V,dwValue);

	dwValue = Str2Int(m_strY1,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_Y,dwValue);
	dwValue = Str2Int(m_strU1,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_U,dwValue);
	dwValue = Str2Int(m_strV1,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1_V,dwValue);

	dwValue = Str2Int(m_strY2,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_Y,dwValue);
	dwValue = Str2Int(m_strU2,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_U,dwValue);
	dwValue = Str2Int(m_strV2,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2_V,dwValue);

	dwValue = Str2Int(m_strY3,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_Y,dwValue);
	dwValue = Str2Int(m_strU3,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_U,dwValue);
	dwValue = Str2Int(m_strV3,16);
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3_V,dwValue);
	UpdateData(FALSE);

	//ColorButton
	dwValue = m_btnColor0.GetColor();
	m_pGOSGMgr->SetItemValue(GOSD_COLOR0,dwValue);
	dwValue = m_btnColor1.GetColor();
	m_pGOSGMgr->SetItemValue(GOSD_COLOR1,dwValue);
	dwValue = m_btnColor2.GetColor();
	m_pGOSGMgr->SetItemValue(GOSD_COLOR2,dwValue);
	dwValue = m_btnColor3.GetColor();
	m_pGOSGMgr->SetItemValue(GOSD_COLOR3,dwValue);

	//Check Box
	BYTE byGOSD = 0;
	for (int i = 0; i < 16; i++)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(m_dwCheckCtrlID[i]);
		if (pWnd && pWnd->GetSafeHwnd())
		{
			CButton* pCheck = dynamic_cast<CButton*>(pWnd);
			int nValue = pCheck->GetCheck();
			if (nValue == 1)
				SetBit(byGOSD,i);
		}
	}
	m_pGOSGMgr->SetItemValue(GOSD_GPIO_SEL,byGOSD);
}

void C_GOSDParameterDlg::UpdateModeltoUI()
{
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return;
	//ComBox
	m_comBoxEnable.SetCurSel(m_pGOSGMgr->GetItemValue(GOSD_ENABLE));
	m_comBoxTrigMode.SetCurSel(m_pGOSGMgr->GetItemValue(GOSD_TRIG_MODE));
	int nMode = m_pGOSGMgr->GetItemValue(GOSD_BIT_MODE);
	m_comBoxBitMode.SetCurSel(nMode);
	UpdateClrGroup(nMode);
	m_comBoxScale.SetCurSel(m_pGOSGMgr->GetItemValue(GOSD_SCALE_VALUE));
	m_comBoxBKTieYUV.SetCurSel(m_pGOSGMgr->GetItemValue(GOSD_BK_TIE_YUV));

	//Edit
	UpdateData(TRUE);
	DWORD dwValue = 0;
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_RES_X);
	m_strResX.Format(_T("%d"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_RES_Y);
	m_strResY.Format(_T("%d"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_START_X);
	m_strStartX.Format(_T("%d"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_START_Y);
	m_strStartY.Format(_T("%d"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_SIZE_W);
	m_strSizeW.Format(_T("%d"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_SIZE_H);
	m_strSizeH.Format(_T("%d"),dwValue);

	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR0_Y);
	m_strY0.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR0_U);
	m_strU0.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR0_V);
	m_strV0.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR1_Y);
	m_strY1.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR1_U);
	m_strU1.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR1_V);
	m_strV1.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR2_Y);
	m_strY2.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR2_U);
	m_strU2.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR2_V);
	m_strV2.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR3_Y);
	m_strY3.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR3_U);
	m_strU3.Format(_T("%X"),dwValue);
	dwValue = m_pGOSGMgr->GetItemValue(GOSD_COLOR3_V);
	m_strV3.Format(_T("%X"),dwValue);

	UpdateData(FALSE);

	//Color Button
	m_btnColor0.SetColor(m_pGOSGMgr->GetItemValue(GOSD_COLOR0));
	m_btnColor1.SetColor(m_pGOSGMgr->GetItemValue(GOSD_COLOR1));
	m_btnColor2.SetColor(m_pGOSGMgr->GetItemValue(GOSD_COLOR2));
	m_btnColor3.SetColor(m_pGOSGMgr->GetItemValue(GOSD_COLOR3));

	//CkeckBox
	BYTE byGPIO = m_pGOSGMgr->GetItemValue(GOSD_GPIO_SEL);
	BYTE byTemp = byGPIO;
	for (int i = 0; i < 16; i++)
	{
		byGPIO = byTemp;
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(m_dwCheckCtrlID[i]);
		if (pWnd && pWnd->GetSafeHwnd())
		{
			CButton* pCheck = dynamic_cast<CButton*>(pWnd);
			pCheck->SetCheck(GetBit(byGPIO,i));
		}
	}
}

BEGIN_MESSAGE_MAP(C_GOSDParameterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CLR0, &C_GOSDParameterDlg::OnBnClickedButtonClr0)
	ON_BN_CLICKED(IDC_BUTTON_CLR1, &C_GOSDParameterDlg::OnBnClickedButtonClr1)
	ON_BN_CLICKED(IDC_BUTTON_CLR2, &C_GOSDParameterDlg::OnBnClickedButtonClr2)
	ON_BN_CLICKED(IDC_BUTTON_CLR3, &C_GOSDParameterDlg::OnBnClickedButtonClr3)
	ON_BN_CLICKED(IDC_BUTTON_SET_CENTER, &C_GOSDParameterDlg::OnBnClickedButtonSetCenter)

	ON_EN_KILLFOCUS(IDC_EDIT_RES_X, &C_GOSDParameterDlg::OnEnKillfocusEditResX)
	ON_EN_KILLFOCUS(IDC_EDIT_RES_Y, &C_GOSDParameterDlg::OnEnKillfocusEditResY)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTX, &C_GOSDParameterDlg::OnEnKillfocusEditStartx)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTY, &C_GOSDParameterDlg::OnEnKillfocusEditStarty)
	ON_EN_KILLFOCUS(IDC_EDIT_SIZEW, &C_GOSDParameterDlg::OnEnKillfocusEditSizew)
	ON_EN_KILLFOCUS(IDC_EDIT_SIZEH, &C_GOSDParameterDlg::OnEnKillfocusEditSizeh)
	ON_CBN_SELCHANGE(IDC_COMBO_BITMODE2, &C_GOSDParameterDlg::OnCbnSelchangeComboBitmode2)
	ON_EN_CHANGE(IDC_EDIT_Y0, &C_GOSDParameterDlg::OnEnChangeEditY0)
	ON_EN_CHANGE(IDC_EDIT_U0, &C_GOSDParameterDlg::OnEnChangeEditU0)
	ON_EN_CHANGE(IDC_EDIT_V0, &C_GOSDParameterDlg::OnEnChangeEditV0)
	ON_EN_CHANGE(IDC_EDIT_Y1, &C_GOSDParameterDlg::OnEnChangeEditY1)
	ON_EN_CHANGE(IDC_EDIT_U1, &C_GOSDParameterDlg::OnEnChangeEditU1)
	ON_EN_CHANGE(IDC_EDIT_V1, &C_GOSDParameterDlg::OnEnChangeEditV1)
	ON_EN_CHANGE(IDC_EDIT_Y2, &C_GOSDParameterDlg::OnEnChangeEditY2)
	ON_EN_CHANGE(IDC_EDIT_U2, &C_GOSDParameterDlg::OnEnChangeEditU2)
	ON_EN_CHANGE(IDC_EDIT_V2, &C_GOSDParameterDlg::OnEnChangeEditV2)
	ON_EN_CHANGE(IDC_EDIT_Y3, &C_GOSDParameterDlg::OnEnChangeEditY3)
	ON_EN_CHANGE(IDC_EDIT_U3, &C_GOSDParameterDlg::OnEnChangeEditU3)
	ON_EN_CHANGE(IDC_EDIT_V3, &C_GOSDParameterDlg::OnEnChangeEditV3)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_GOSDPARAMETERS, &C_GOSDParameterDlg::OnBnClickedButtonSaveGosdparameters)
END_MESSAGE_MAP()


// C_GOSDParameter message handlers
void C_GOSDParameterDlg::SaveCheckCtrlID()
{
	m_dwCheckCtrlID[0] = m_Ckeck0.GetDlgCtrlID();
	m_dwCheckCtrlID[1] = m_Ckeck1.GetDlgCtrlID();
	m_dwCheckCtrlID[2] = m_Ckeck2.GetDlgCtrlID();
	m_dwCheckCtrlID[3] = m_Ckeck3.GetDlgCtrlID();
	m_dwCheckCtrlID[4] = m_Ckeck4.GetDlgCtrlID();
	m_dwCheckCtrlID[5] = m_Ckeck5.GetDlgCtrlID();
	m_dwCheckCtrlID[6] = m_Ckeck6.GetDlgCtrlID();
	m_dwCheckCtrlID[7] = m_Ckeck7.GetDlgCtrlID();
	m_dwCheckCtrlID[8] = m_Ckeck8.GetDlgCtrlID();
	m_dwCheckCtrlID[9] = m_Ckeck9.GetDlgCtrlID();
	m_dwCheckCtrlID[10] = m_Ckeck10.GetDlgCtrlID();
	m_dwCheckCtrlID[11] = m_Ckeck11.GetDlgCtrlID();
	m_dwCheckCtrlID[12] = m_Ckeck12.GetDlgCtrlID();
	m_dwCheckCtrlID[13] = m_Ckeck13.GetDlgCtrlID();
	m_dwCheckCtrlID[14] = m_Ckeck14.GetDlgCtrlID();
	m_dwCheckCtrlID[15] = m_Ckeck15.GetDlgCtrlID();
}


void C_GOSDParameterDlg::InitCheckEnable()
{
	for (int i = 0; i < 16; i++)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(m_dwCheckCtrlID[i]);
		if (pWnd && pWnd->GetSafeHwnd())
		{
			if (i>7)
			{
				pWnd->ShowWindow(FALSE);
			}
			pWnd->EnableWindow(m_nCheckEnable[i]);
		}
	}
}

void C_GOSDParameterDlg::AddComBoxItem()
{
	m_comBoxEnable.AddString(_T("Disable"));
	m_comBoxEnable.AddString(_T("Enable"));

	m_comBoxTrigMode.AddString(_T("Low Level"));
	m_comBoxTrigMode.AddString(_T("High Level"));

	m_comBoxBitMode.AddString(_T("1-Bit"));
	m_comBoxBitMode.AddString(_T("2-Bit"));

	m_comBoxScale.AddString(_T("1x1"));
	m_comBoxScale.AddString(_T("2x2"));

	m_comBoxBKTieYUV.AddString(_T("Disable"));
	m_comBoxBKTieYUV.AddString(_T("Enable"));
}

void C_GOSDParameterDlg::InitCtrlIDVector()
{
	m_vecCtrlID.push_back(GetDlgItem(IDC_COMBO_ENABLE)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_COMBO_TRIGMODE)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_COMBO_BITMODE2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_COMBO_SCALE)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_COMBO_BK_TIE_YUV)->GetDlgCtrlID());

	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_RES_X)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_RES_Y)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_STARTX)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_STARTY)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_SIZEW)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_SIZEH)->GetDlgCtrlID());

	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_Y0)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_U0)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_V0)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_Y1)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_U1)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_V1)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_Y2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_U2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_V2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_Y3)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_U3)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_EDIT_V3)->GetDlgCtrlID());

	m_vecCtrlID.push_back(GetDlgItem(IDC_BUTTON_CLR0)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_BUTTON_CLR1)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_BUTTON_CLR2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_BUTTON_CLR3)->GetDlgCtrlID());

	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK0)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK1)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK2)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK3)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK4)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK5)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK6)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK7)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK8)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK9)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK10)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK11)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK12)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK13)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK14)->GetDlgCtrlID());
	m_vecCtrlID.push_back(GetDlgItem(IDC_CHECK15)->GetDlgCtrlID());
}

void C_GOSDParameterDlg::InitDescMap()
{
	//Combox
	m_mapCtrlTips[GetDlgItem(IDC_COMBO_ENABLE)->GetDlgCtrlID()] = _T("0: Disable GOSD, 1: Enable GOSD");
	m_mapCtrlTips[GetDlgItem(IDC_COMBO_TRIGMODE)->GetDlgCtrlID()] = _T("0: Low level trigger, 1: High level trigger");
	m_mapCtrlTips[GetDlgItem(IDC_COMBO_BITMODE2)->GetDlgCtrlID()] = _T("0: One Bit Mode, 1: Two Bits Mode");
	m_mapCtrlTips[GetDlgItem(IDC_COMBO_SCALE)->GetDlgCtrlID()] = _T("0: 1x1 (X times Y), 1: GOSD scaling up 2x2 (2X times 2Y)");
	m_mapCtrlTips[GetDlgItem(IDC_COMBO_BK_TIE_YUV)->GetDlgCtrlID()] = _T("0: Disable tie YUV function, 1: Enable tie YUV function");
	
	//Edit
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_RES_X)->GetDlgCtrlID()] = _T("Resolution X");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_RES_Y)->GetDlgCtrlID()] = _T("Resolution Y");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_STARTX)->GetDlgCtrlID()] = _T("GOSD start point of x coordinate (Start at 1), Only start with odd pixel for YUV 422 sharing UV ");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_STARTY)->GetDlgCtrlID()] = _T("GOSD start point of y coordinate (Start at 1)");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_SIZEW)->GetDlgCtrlID()] = _T("GOSD width, must be multiple of 64");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_SIZEH)->GetDlgCtrlID()] = _T("GOSD height");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_Y0)->GetDlgCtrlID()] = _T("Color index 0 Y image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_U0)->GetDlgCtrlID()] = _T("Color index 0 U image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_V0)->GetDlgCtrlID()] = _T("Color index 0 V image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_Y1)->GetDlgCtrlID()] = _T("Color index 1 Y image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_U1)->GetDlgCtrlID()] = _T("Color index 1 U image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_V1)->GetDlgCtrlID()] = _T("Color index 1 V image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_Y2)->GetDlgCtrlID()] = _T("Color index 2 Y image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_U2)->GetDlgCtrlID()] = _T("Color index 2 U image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_V2)->GetDlgCtrlID()] = _T("Color index 2 V image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_Y3)->GetDlgCtrlID()] = _T("Color index 3 Y image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_U3)->GetDlgCtrlID()] = _T("Color index 3 U image");
	m_mapCtrlTips[GetDlgItem(IDC_EDIT_V3)->GetDlgCtrlID()] = _T("Color index 3 V image");

	//Color Button
	m_mapCtrlTips[GetDlgItem(IDC_BUTTON_CLR0)->GetDlgCtrlID()] = _T("Click Chose Color1");
	m_mapCtrlTips[GetDlgItem(IDC_BUTTON_CLR1)->GetDlgCtrlID()] = _T("Click Chose Color2");
	m_mapCtrlTips[GetDlgItem(IDC_BUTTON_CLR2)->GetDlgCtrlID()] = _T("Click Chose Color3");
	m_mapCtrlTips[GetDlgItem(IDC_BUTTON_CLR3)->GetDlgCtrlID()] = _T("Click Chose Color4");

	//Checkbox
	m_mapCtrlTips[GetDlgItem(IDC_CHECK0)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK1)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK2)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK3)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK4)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK5)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK6)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK7)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK8)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK9)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK10)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK11)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK12)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK13)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK14)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
	m_mapCtrlTips[GetDlgItem(IDC_CHECK15)->GetDlgCtrlID()] = _T("GPIO input trigger selection");
}

void C_GOSDParameterDlg::ShowDescByPoint(CPoint point)
{
	for (int i = 0; i < m_vecCtrlID.size(); i++)
	{
		CWnd* pWnd = NULL;
		DWORD dwCtrlID = m_vecCtrlID[i];
		pWnd = GetDlgItem(dwCtrlID);
		if (pWnd && pWnd->GetSafeHwnd())
		{
			CRect rcCtrl;
			pWnd->GetWindowRect(&rcCtrl);
			if (rcCtrl.PtInRect(point))
			{
				CString strDescriptor = _T("");
				std::map<DWORD,CString>::iterator itEnd = m_mapCtrlTips.end();
				std::map<DWORD,CString>::iterator it = m_mapCtrlTips.find(dwCtrlID);
				if (it != itEnd)
				{
					strDescriptor = it->second;
				}
				pWnd = GetDlgItem(IDC_STATIC_DESCTEXT);
				pWnd->SetWindowText(strDescriptor);
			}
		}
	}
}

void C_GOSDParameterDlg::OnBnClickedButtonClr0()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	int nY = Str2Int(m_strY0,16);
	int nU = Str2Int(m_strU0,16);
	int nV = Str2Int(m_strV0,16);
	clrDlg.InitByYUV(nY,nU,nV);
	if (IDOK == clrDlg.DoModal())
	{
		UpdateData(TRUE);
		clrDlg.GetYUV(&nY,&nU,&nV);
		m_strY0.Format(_T("%X"),nY);
		m_strU0.Format(_T("%X"),nU);
		m_strV0.Format(_T("%X"),nV);

		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor0.SetColor(clrValue);
		UpdateData(FALSE);
	}
}

void C_GOSDParameterDlg::OnBnClickedButtonClr1()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	int nY = Str2Int(m_strY1,16);
	int nU = Str2Int(m_strU1,16);
	int nV = Str2Int(m_strV1,16);
	clrDlg.InitByYUV(nY,nU,nV);
	if (IDOK == clrDlg.DoModal())
	{
		UpdateData(TRUE);
		clrDlg.GetYUV(&nY,&nU,&nV);
		m_strY1.Format(_T("%X"),nY);
		m_strU1.Format(_T("%X"),nU);
		m_strV1.Format(_T("%X"),nV);

		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor1.SetColor(clrValue);
		UpdateData(FALSE);
	}
}

void C_GOSDParameterDlg::OnBnClickedButtonClr2()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	int nY = Str2Int(m_strY2,16);
	int nU = Str2Int(m_strU2,16);
	int nV = Str2Int(m_strV2,16);
	clrDlg.InitByYUV(nY,nU,nV);
	if (IDOK == clrDlg.DoModal())
	{
		UpdateData(TRUE);
		clrDlg.GetYUV(&nY,&nU,&nV);
		m_strY2.Format(_T("%X"),nY);
		m_strU2.Format(_T("%X"),nU);
		m_strV2.Format(_T("%X"),nV);

		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor2.SetColor(clrValue);
		UpdateData(FALSE);
	}
}

void C_GOSDParameterDlg::OnBnClickedButtonClr3()
{
	// TODO: Add your control notification handler code here
	C_MyColorDlg clrDlg;
	int nY = Str2Int(m_strY3,16);
	int nU = Str2Int(m_strU3,16);
	int nV = Str2Int(m_strV3,16);
	clrDlg.InitByYUV(nY,nU,nV);
	if (IDOK == clrDlg.DoModal())
	{
		UpdateData(TRUE);
		clrDlg.GetYUV(&nY,&nU,&nV);
		m_strY3.Format(_T("%X"),nY);
		m_strU3.Format(_T("%X"),nU);
		m_strV3.Format(_T("%X"),nV);

		COLORREF clrValue = clrDlg.GetColor();
		m_btnColor3.SetColor(clrValue);
		UpdateData(FALSE);
	}
}

void C_GOSDParameterDlg::OnBnClickedButtonSetCenter()
{
	// TODO: Add your control notification handler code here
	int nStartX = 0;
	int nStartY = 0;
	UpdateData(TRUE);
	DWORD nScaleValue = m_comBoxScale.GetCurSel();
	int nResX = Str2Int(m_strResX,10);
	int nResY = Str2Int(m_strResY,10);
	int nSizeW = Str2Int(m_strSizeW,10);
	int nSizeH = Str2Int(m_strSizeH,10);

	nStartX = (nResX - (nSizeW *(nScaleValue + 1)))/2 + 1;
	m_strStartX.Format(_T("%d"),nStartX);
	nStartY = (nResY - (nSizeH *(nScaleValue + 1)))/2 + 1;
	m_strStartY.Format(_T("%d"),nStartY);
	UpdateData(FALSE);
}
void C_GOSDParameterDlg::OnEnKillfocusEditResX()
{
	// TODO: Add your control notification handler code here
	m_nLastResX = Str2Int(m_strResX,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strResX,10);
	if (nCurrent == 0)
	{
		AfxMessageBox(_T("The Resolution_X value must is not zero."));
		m_strResX.Format(_T("%d"),m_nLastResX);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Resolution_X value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strResX.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnKillfocusEditResY()
{
	// TODO: Add your control notification handler code here
	m_nLastResY = Str2Int(m_strResY,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strResY,10);
	if (nCurrent == 0)
	{
		AfxMessageBox(_T("The Resolution_Y value must is not zero."));
		m_strResY.Format(_T("%d"),m_nLastResY);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Resolution_Y value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strResY.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnKillfocusEditStartx()
{
	// TODO: Add your control notification handler code here
	m_nLastStartX = Str2Int(m_strStartX,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strStartX,10);
	if (nCurrent == 0)
	{
		AfxMessageBox(_T("The Start_X value must is not zero."));
		m_strStartX.Format(_T("%d"),m_nLastStartX);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Start_X value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strStartX.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnKillfocusEditStarty()
{
	// TODO: Add your control notification handler code here
	m_nLastStartY = Str2Int(m_strStartY,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strStartY,10);
	if (nCurrent == 0)
	{
		AfxMessageBox(_T("The Start_Y value must is not zero."));
		m_strStartY.Format(_T("%d"),m_nLastStartY);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Start_Y value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strStartY.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnKillfocusEditSizew()
{
	// TODO: Add your control notification handler code here
	m_nLastSizeW = Str2Int(m_strSizeW,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strSizeW,10);
	if ((nCurrent%64) != 0 || nCurrent == 0)
	{
		AfxMessageBox(_T("The Size_W value must multiple of 64."));
		m_strSizeW.Format(_T("%d"),m_nLastSizeW);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Size_W value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strSizeW.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnKillfocusEditSizeh()
{
	// TODO: Add your control notification handler code here
	m_nLastSizeH = Str2Int(m_strSizeH,10);
	UpdateData(TRUE);
	int nCurrent = Str2Int(m_strSizeH,10);
	if (nCurrent == 0)
	{
		AfxMessageBox(_T("The Size_H value must is not zero."));
		m_strSizeH.Format(_T("%d"),m_nLastSizeH);
	}
	if (nCurrent > 0xFFFF)
	{
		AfxMessageBox(_T("The Size_H value range: 0 - 65535."));
		while(nCurrent > 0xFFFF)
		{
			nCurrent /= 10;
		}
		m_strSizeH.Format(_T("%d"),nCurrent);
	}
	UpdateData(FALSE);
}


int  C_GOSDParameterDlg::Str2Int(CString strHexString,UINT uSysNum)
{
	int nResult = 0;
	if (strHexString.GetLength() != 0)
	{
		char pBuf[MAX_PATH] = {0};
		USES_CONVERSION;
		strcpy_s(pBuf,T2A(strHexString));
		nResult = strtol(pBuf,NULL,uSysNum);
	}
	
	return nResult;
}
void C_GOSDParameterDlg::OnCbnSelchangeComboBitmode2()
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_comBoxBitMode.GetCurSel();
	if (nCurSel == -1)
		return;

	switch(nCurSel)
	{
	case 0:
		UpdateClrGroup(nCurSel);
		break;
	case 1:
		UpdateClrGroup(nCurSel);
		break;
	default:
		break;
	}
}

void C_GOSDParameterDlg::UpdateClrGroup(int nMode)
{
	if (nMode != 0 && nMode != 1)
		return;

	DWORD CtrlIDArray[] =
	{
		GetDlgItem(IDC_STATIC_GROUP2)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_Y2)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_U2)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_V2)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_Y2)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_U2)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_V2)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_CLR2)->GetDlgCtrlID(),

		GetDlgItem(IDC_STATIC_GROUP3)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_Y3)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_U3)->GetDlgCtrlID(),
		GetDlgItem(IDC_STATIC_V3)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_Y3)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_U3)->GetDlgCtrlID(),
		GetDlgItem(IDC_EDIT_V3)->GetDlgCtrlID(),
		GetDlgItem(IDC_BUTTON_CLR3)->GetDlgCtrlID(),
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

void C_GOSDParameterDlg::OnEnChangeEditY0()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strY0;
	UpdateData(TRUE);
	if (Str2Int(m_strY0,16) > 255 || Str2Int(m_strY0,16) < 0)
	{
		m_strY0 = strOldValue;
	}
	int nY = Str2Int(m_strY0,16);
	int nU = Str2Int(m_strU0,16);
	int nV = Str2Int(m_strV0,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor0.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditU0()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strU0;
	UpdateData(TRUE);
	if (Str2Int(m_strU0,16) > 255 /*|| Str2Int(m_strU0,16) < 128*/)
	{
		m_strU0 = strOldValue;
	}
	int nY = Str2Int(m_strY0,16);
	int nU = Str2Int(m_strU0,16);
	int nV = Str2Int(m_strV0,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor0.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditV0()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strV0;
	UpdateData(TRUE);
	if (Str2Int(m_strV0,16) > 255 /*|| Str2Int(m_strV0,16) < 128*/)
	{
		m_strV0 = strOldValue;
	}
	int nY = Str2Int(m_strY0,16);
	int nU = Str2Int(m_strU0,16);
	int nV = Str2Int(m_strV0,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor0.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditY1()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strY1;
	UpdateData(TRUE);
	if (Str2Int(m_strY1,16) > 255 || Str2Int(m_strY1,16) < 0)
	{
		m_strY1 = strOldValue;
	}
	int nY = Str2Int(m_strY1,16);
	int nU = Str2Int(m_strU1,16);
	int nV = Str2Int(m_strV1,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor1.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditU1()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strU1;
	UpdateData(TRUE);
	if (Str2Int(m_strU1,16) > 255 /*|| Str2Int(m_strU1,16) < 128*/)
	{
		m_strU1 = strOldValue;
	}
	int nY = Str2Int(m_strY1,16);
	int nU = Str2Int(m_strU1,16);
	int nV = Str2Int(m_strV1,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor1.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditV1()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strV1;
	UpdateData(TRUE);
	if (Str2Int(m_strV1,16) > 255 /*|| Str2Int(m_strV1,16) < 128*/)
	{
		m_strV1 = strOldValue;
	}
	int nY = Str2Int(m_strY1,16);
	int nU = Str2Int(m_strU1,16);
	int nV = Str2Int(m_strV1,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor1.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditY2()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strY2;
	UpdateData(TRUE);
	if (Str2Int(m_strY2,16) > 255 || Str2Int(m_strY2,16) < 0)
	{
		m_strY2 = strOldValue;
	}
	int nY = Str2Int(m_strY2,16);
	int nU = Str2Int(m_strU2,16);
	int nV = Str2Int(m_strV2,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor2.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditU2()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strU2;
	UpdateData(TRUE);
	if (Str2Int(m_strU2,16) > 255 /*|| Str2Int(m_strU2,16) < 128*/)
	{
		m_strU2 = strOldValue;
	}
	int nY = Str2Int(m_strY2,16);
	int nU = Str2Int(m_strU2,16);
	int nV = Str2Int(m_strV2,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor2.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditV2()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strV2;
	UpdateData(TRUE);
	if (Str2Int(m_strV2,16) > 255 /*|| Str2Int(m_strV2,16) < 128*/)
	{
		m_strV2 = strOldValue;
	}
	int nY = Str2Int(m_strY2,16);
	int nU = Str2Int(m_strU2,16);
	int nV = Str2Int(m_strV2,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor2.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditY3()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strY3;
	UpdateData(TRUE);
	if (Str2Int(m_strY3,16) > 255 || Str2Int(m_strY3,16) < 0)
	{
		m_strY3 = strOldValue;
	}
	int nY = Str2Int(m_strY3,16);
	int nU = Str2Int(m_strU3,16);
	int nV = Str2Int(m_strV3,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor3.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditU3()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strU3;
	UpdateData(TRUE);
	if (Str2Int(m_strU3,16) > 255 /*|| Str2Int(m_strU3,16) < 128*/)
	{
		m_strU3 = strOldValue;
	}
	int nY = Str2Int(m_strY3,16);
	int nU = Str2Int(m_strU3,16);
	int nV = Str2Int(m_strV3,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor3.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnEnChangeEditV3()
{
	// TODO:  Add your control notification handler code here
	CString strOldValue = m_strV3;
	UpdateData(TRUE);
	if (Str2Int(m_strV3,16) > 255 /*|| Str2Int(m_strV3,16) < 128*/)
	{
		m_strV3 = strOldValue;
	}
	int nY = Str2Int(m_strY3,16);
	int nU = Str2Int(m_strU3,16);
	int nV = Str2Int(m_strV3,16);
	COLORREF NewClr = m_pGOSGMgr->GetClrByYUV(nY,nU,nV);
	m_btnColor3.SetColor(NewClr);
	UpdateData(FALSE);
}

void C_GOSDParameterDlg::OnBnClickedButtonSaveGosdparameters()
{
	// TODO: Add your control notification handler code here
	CString strConfigFilePath = _T("");
	CBurningToolGOSDCreatorApp* pGOSDCreatorApp = (CBurningToolGOSDCreatorApp*)AfxGetApp();
	if (!pGOSDCreatorApp->m_strBurPrjPath.IsEmpty())
	{
		strConfigFilePath = pGOSDCreatorApp->m_strBurPrjPath + _T("\\GOSD_Parameters.txt");
	}
	else
	{
		CString strFolder = _T("");
		strFolder = SelFolderPath();
		if (strFolder.GetLength() > 0)
		{
			strConfigFilePath = strFolder + _T("\\GOSD_Parameters.txt");
		}
	}
	if (strConfigFilePath.GetLength() > 0)
	{
		SaveUItoMadel();//for save config file

		if (AfxMessageBox(_T("Section28 will be update."),MB_OKCANCEL) != IDOK)
			return;

		if (!SaveConfigFile(strConfigFilePath))
			AfxMessageBox(_T("Save GOSD Parameters Failed."));

		int nState = m_comBoxEnable.GetCurSel();
		if (nState < 0)//error
			nState = 0;
		CString strBurToolTitle = pGOSDCreatorApp->m_strBurToolTitle;
		CWnd* pBurToolWnd = FindWindow(NULL,strBurToolTitle);
		if(pBurToolWnd)
			::SendMessage(pBurToolWnd->m_hWnd,WM_UPDATE_GOSD_TO_CUSTOM,(WPARAM)nState,0);
		else
			AfxMessageBox(_T("Update GOSD Parameters to Custom Failed."));
	}
}

CString C_GOSDParameterDlg::SelFolderPath()
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

BOOL C_GOSDParameterDlg::SaveConfigFile(CString strTxtFilePath)
{
	ASSERT(m_pGOSGMgr);
	if (!m_pGOSGMgr)
		return FALSE;

	DWORD   dwValue = 0;
	CString strSection = _T("General");
	CString strKey     = _T("");
	CString strText    = _T("");

	CString strKeyArray[] = 
	{
		//Custom
		GOSD_COLOR0,
		GOSD_COLOR1,
		GOSD_COLOR2,
		GOSD_COLOR3,
		//Init
		GOSD_ENABLE,
		GOSD_COLOR0_Y,
		GOSD_COLOR0_U,
		GOSD_COLOR0_V,
		GOSD_COLOR1_Y,
		GOSD_COLOR1_U,
		GOSD_COLOR1_V,
		GOSD_COLOR2_Y,
		GOSD_COLOR2_U,
		GOSD_COLOR2_V,
		GOSD_COLOR3_Y,
		GOSD_COLOR3_U,
		GOSD_COLOR3_V,

		GOSD_RES_X,
		GOSD_RES_Y,	
		GOSD_START_X,
		GOSD_START_Y,
		GOSD_SIZE_W,
		GOSD_SIZE_H,
		GOSD_GPIO_SEL,	
		GOSD_TRIG_MODE,	
		GOSD_BIT_MODE,	
		GOSD_SCALE_VALUE,
		GOSD_BK_TIE_YUV		
	};

	for (int i = 0; i < sizeof(strKeyArray)/sizeof(strKeyArray[0]); i++)
	{
		strKey = strKeyArray[i];

		dwValue = m_pGOSGMgr->GetItemValue(strKey);
		strText.Format(_T("%d"),dwValue);
		if(!::WritePrivateProfileString(strSection, strKey, strText, strTxtFilePath))
			return FALSE;
	}

	return TRUE;
}
