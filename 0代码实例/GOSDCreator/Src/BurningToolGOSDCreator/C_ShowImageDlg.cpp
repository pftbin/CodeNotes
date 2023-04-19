// C_ShowImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BurningToolGOSDCreator.h"
#include "C_ShowImageDlg.h"

// C_ShowImageDlg dialog

IMPLEMENT_DYNAMIC(C_ShowImageDlg, CDialog)

C_ShowImageDlg::C_ShowImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C_ShowImageDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

C_ShowImageDlg::~C_ShowImageDlg()
{
}

void C_ShowImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL C_ShowImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_scrollHinfo.cbSize = sizeof(m_scrollHinfo);
	m_scrollHinfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	m_scrollHinfo.nMax = 100;
	m_scrollHinfo.nMin = 0;
	m_scrollHinfo.nPos = 0;
	m_scrollHinfo.nPage = 10;
	SetScrollInfo(SB_HORZ,&m_scrollHinfo,SIF_ALL);

	m_scrollVinfo.cbSize = sizeof(m_scrollVinfo);
	m_scrollVinfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	m_scrollVinfo.nMax = 100;
	m_scrollVinfo.nMin = 0;
	m_scrollVinfo.nPos = 0;
	m_scrollVinfo.nPage = 10;
	SetScrollInfo(SB_VERT,&m_scrollVinfo,SIF_ALL);


	ShowScrollBar(SB_VERT,FALSE);
	ShowScrollBar(SB_HORZ,FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(C_ShowImageDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// C_ShowImageDlg message handlers
void C_ShowImageDlg::OnPaint()
{
	CDialog::OnPaint();

	ShowImage();
}

BOOL C_ShowImageDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void  C_ShowImageDlg::SetImagePath(CString strImagePath)
{
	m_strImagePath = strImagePath;
}

void C_ShowImageDlg::ShowImage()
{
// 	HBITMAP hBitmap;
// 	BITMAP bm;
// 	hBitmap=(HBITMAP)LoadImage(AfxGetInstanceHandle(),m_strImagePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION); GetObject(hBitmap, sizeof BITMAP, &bm);

	Bitmap img(m_strImagePath);//Load Image
	m_nImageW = img.GetWidth();
	m_nImageH = img.GetHeight();
	if (m_nImageW <= 0 || m_nImageH <= 0)
		return;
	Color	clrBackgnd;
	img.GetHBITMAP(clrBackgnd,&m_hBitmap);


	HDC hSrcDC,hDesDC;
	hDesDC = GetDC()->m_hDC;
	CRect rect;
	GetClientRect(&rect);
	HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));
	::FillRect(hDesDC,&rect,hBrush);

	int nWidth = rect.Width();
	int nHeight = rect.Height();
	if (nWidth < m_nImageW)
	{
		ShowScrollBar(SB_HORZ,TRUE);	
	}
	if (nHeight < m_nImageH)
	{
		ShowScrollBar(SB_VERT,TRUE);
	}

	hSrcDC = CreateCompatibleDC(hDesDC);
	SelectObject(hSrcDC, m_hBitmap);
	::SetStretchBltMode(hDesDC,COLORONCOLOR);
	GetScrollInfo(SB_HORZ,&m_scrollHinfo);
	GetScrollInfo(SB_VERT,&m_scrollVinfo);

	int cx = m_scrollHinfo.nPos*(m_nImageW/100);
	int cy = m_scrollVinfo.nPos*(m_nImageH/100);
	::StretchBlt(hDesDC, 5,  5, m_nImageW, m_nImageH,
				 hSrcDC, cx,cy, m_nImageW, m_nImageH, SRCCOPY);
	CRect rcBorder(5,5,m_nImageW-cx+5,m_nImageH-cy+5);
	DrawFocusRect(hDesDC,&rcBorder);
}

void C_ShowImageDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	// TODO: Add your message handler code here and/or call default
	m_scrollHinfo.nMax = 100;
	m_scrollHinfo.nMin = 0;
	int nOffset = 0;
	switch (nSBCode) 
	{ 
	case SB_LINELEFT: 
		m_scrollHinfo.nPos -= 1; 
		if (m_scrollHinfo.nPos<m_scrollHinfo.nMin) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMin; 
			break; 
		} 
		SetScrollInfo(SB_HORZ,&m_scrollHinfo,SIF_ALL); 
		ScrollWindow(1,0); 
		break; 
	case SB_LINERIGHT: 
		m_scrollHinfo.nPos += 1; 
		if (m_scrollHinfo.nPos>m_scrollHinfo.nMax) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMax; 
			break; 
		} 
		SetScrollInfo(SB_HORZ,&m_scrollHinfo,SIF_ALL); 
		ScrollWindow(-1,0);
		break;
		// 	case SB_PAGEUP:    
		// 		scrollinfo.nPos -= 1; 
		// 		if (scrollinfo.nPos<scrollinfo.nMin) 
		// 		{ 
		// 			scrollinfo.nPos = scrollinfo.nMin; 
		// 			break; 
		// 		} 
		// 		SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);    
		// 		break;    
		// 		// 如果向右滚动一页，则pos加10   
		// 	case SB_PAGEDOWN:    
		// 		scrollinfo.nPos += 1; 
		// 		if (scrollinfo.nPos>scrollinfo.nMax) 
		// 		{ 
		// 			scrollinfo.nPos = scrollinfo.nMax; 
		// 			break; 
		// 		} 
		// 		SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);    
		// 		break;
	case SB_THUMBPOSITION: 
		if(m_scrollHinfo.nPos == nPos)
			return;
		if (m_scrollHinfo.nPos>m_scrollHinfo.nMax) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMax; 
			break; 
		}
		if (m_scrollHinfo.nPos<m_scrollHinfo.nMin) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMin; 
			break; 
		}
		nOffset = m_scrollHinfo.nPos - nPos;
		m_scrollHinfo.nPos = nPos; 
		SetScrollInfo(SB_HORZ,&m_scrollHinfo,SIF_ALL); 
		ScrollWindow(nOffset,0); 
		break; 
	case SB_THUMBTRACK: 
		if(m_scrollHinfo.nPos == nPos)
			return;
		if (m_scrollHinfo.nPos>m_scrollHinfo.nMax) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMax; 
			break; 
		}
		if (m_scrollHinfo.nPos<m_scrollHinfo.nMin) 
		{ 
			m_scrollHinfo.nPos = m_scrollHinfo.nMin; 
			break; 
		}
		nOffset = m_scrollHinfo.nPos - nPos;
		m_scrollHinfo.nPos = nPos; 
		SetScrollInfo(SB_HORZ,&m_scrollHinfo,SIF_ALL); 
		ScrollWindow(nOffset,0); 
		break; 
	case SB_ENDSCROLL: 
		break; 
	} 
}

void C_ShowImageDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
	// TODO: Add your message handler code here and/or call default
	m_scrollVinfo.nMax = 100;
	m_scrollVinfo.nMin = 0;
	BOOL bPositive = TRUE;
	int nOffset = 0;
	switch (nSBCode) 
	{ 
	case SB_LINELEFT: 
		m_scrollVinfo.nPos -= 1; 
		if (m_scrollVinfo.nPos<m_scrollVinfo.nMin) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMin; 
			break; 
		} 
		SetScrollInfo(SB_VERT,&m_scrollVinfo,SIF_ALL); 
		ScrollWindow(1,0); 
		break; 
	case SB_LINERIGHT: 
		m_scrollVinfo.nPos += 1; 
		if (m_scrollVinfo.nPos>m_scrollVinfo.nMax) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMax; 
			break; 
		} 
		SetScrollInfo(SB_VERT,&m_scrollVinfo,SIF_ALL); 
		ScrollWindow(-1,0); 
		break; 
		// 	case SB_PAGEUP:    
		// 		scrollinfo.nPos -= 1; 
		// 		if (scrollinfo.nPos<scrollinfo.nMin) 
		// 		{ 
		// 			scrollinfo.nPos = scrollinfo.nMin; 
		// 			break; 
		// 		} 
		// 		SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);    
		// 		break;    
		// 		// 如果向右滚动一页，则pos加10   
		// 	case SB_PAGEDOWN:    
		// 		scrollinfo.nPos += 1; 
		// 		if (scrollinfo.nPos>scrollinfo.nMax) 
		// 		{ 
		// 			scrollinfo.nPos = scrollinfo.nMax; 
		// 			break; 
		// 		} 
		// 		SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);    
		// 		break;    
	case SB_THUMBPOSITION: 
		if(m_scrollVinfo.nPos == nPos)
			return;
		if (m_scrollVinfo.nPos>m_scrollVinfo.nMax) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMax; 
			break; 
		}
		if (m_scrollVinfo.nPos<m_scrollVinfo.nMin) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMin; 
			break; 
		} 
		nOffset = m_scrollVinfo.nPos - nPos;
		m_scrollVinfo.nPos = nPos; 
		SetScrollInfo(SB_VERT,&m_scrollVinfo,SIF_ALL); 
		ScrollWindow(nOffset,0); 
		break; 
	case SB_THUMBTRACK: 
		if(m_scrollVinfo.nPos == nPos)
			return;
		if (m_scrollVinfo.nPos>m_scrollVinfo.nMax) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMax; 
			break; 
		}
		if (m_scrollVinfo.nPos<m_scrollVinfo.nMin) 
		{ 
			m_scrollVinfo.nPos = m_scrollVinfo.nMin; 
			break; 
		}
		nOffset = m_scrollVinfo.nPos - nPos;
		m_scrollVinfo.nPos = nPos; 
		SetScrollInfo(SB_VERT,&m_scrollVinfo,SIF_ALL); 
		ScrollWindow(nOffset,0); 
		break; 
	case SB_ENDSCROLL: 
		break; 
	} 
}


void C_ShowImageDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
 	CRect rcClient;
 	GetClientRect(&rcClient);
	if (rcClient.Width() < 100)
		m_scrollHinfo.nPos = cy;
	if (rcClient.Height() < 100)
		m_scrollVinfo.nPos = cx;
	Invalidate();
}
