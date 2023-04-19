//MapHyperLink.h , MapHyperLink.cpp 

#include "stdafx.h"
#include "MapHyperLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1

char* TCHAR2char( const TCHAR* STR )
{
	int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, FALSE);
	char* str = new char[sizeof(char) * size];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, str, size, NULL, FALSE);

	return str;
}

TCHAR* char2TCAHR( const char* str )
{
	int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	TCHAR* retStr = new TCHAR[size * sizeof(TCHAR)];
	MultiByteToWideChar(CP_ACP, 0, str, -1, retStr, size);

	return retStr;
}

/////////////////////////////////////////////////////////////////////////////
// CMapHyperLink

CMapHyperLink::CMapHyperLink()
{
    m_hLinkCursor       = NULL;                 // No cursor as yet
	m_bOverControl      = FALSE;                // Cursor not yet over control
    m_bVisited          = FALSE;                // Hasn't been visited yet.
	m_bAdjustToFit      = TRUE;                 // Resize the window to fit the text?
    m_strURL.Empty();
	m_strTipText.Empty();
}

CMapHyperLink::~CMapHyperLink()
{

}

BEGIN_MESSAGE_MAP(CMapHyperLink, CStatic)
    //{{AFX_MSG_MAP(CMapHyperLink)
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapHyperLink message handlers

BOOL CMapHyperLink::PreTranslateMessage(MSG* pMsg) 
{
    m_ToolTip.RelayEvent(pMsg);
    return CStatic::PreTranslateMessage(pMsg);
}

void CMapHyperLink::OnClicked()
{
    int result = (int)GotoURL(m_strURL, SW_SHOW);
    m_bVisited = (result > HINSTANCE_ERROR);
    if (!m_bVisited) {
        MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
        ReportError(result);
    } else 
        SetVisited();                        // Repaint to show visited colour
}

void CMapHyperLink::OnMouseMove(UINT nFlags, CPoint point) 
{
    CStatic::OnMouseMove(nFlags, point);

    if (m_bOverControl)        // Cursor is currently over control
    {
        CRect rect;
        GetClientRect(rect);

        if (!rect.PtInRect(point))
        {
            m_bOverControl = FALSE;
            ReleaseCapture();
            RedrawWindow();
            return;
        }
    }
    else                      // Cursor has just moved over control
    {
        m_bOverControl = TRUE;
        RedrawWindow();
        SetCapture();
    }
}

BOOL CMapHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/) 
{
    if (m_hLinkCursor)
    {
        ::SetCursor(m_hLinkCursor);
        return TRUE;
    }
    return FALSE;
}

void CMapHyperLink::PreSubclassWindow() 
{
    // We want to get mouse clicks via STN_CLICKED
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    
	SetDefaultCursor();      // Try and load up a "hand" cursor

    // Create the tooltip
    CRect rect; 
	GetClientRect(rect);
	m_ToolTip.Create(this);
	if (m_strTipText.IsEmpty())
	{
		m_strTipText = m_strURL;
	}
	m_ToolTip.AddTool(this, m_strTipText, rect, TOOLTIP_ID);

    CStatic::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMapHyperLink operations

void CMapHyperLink::SetURL(CString strURL)
{
    m_strURL = strURL;

    if (::IsWindow(GetSafeHwnd())) {
        PositionWindow();
        if (m_strTipText.IsEmpty())
		{
			m_strTipText = strURL;
		}
		m_ToolTip.UpdateTipText(m_strTipText, this, TOOLTIP_ID);
    }
}

CString CMapHyperLink::GetURL() const
{ 
    return m_strURL;   
}

void CMapHyperLink::SetTipText(CString strTipText)
{
    m_strTipText = strTipText;

    if (::IsWindow(GetSafeHwnd())) {
        PositionWindow();
		m_ToolTip.UpdateTipText(m_strTipText, this, TOOLTIP_ID);
    }
}

CString CMapHyperLink::GetTipText() const
{ 
    return m_strTipText;   
}

void CMapHyperLink::SetVisited(BOOL bVisited /* = TRUE */) 
{ 
    m_bVisited = bVisited; 

    if (::IsWindow(GetSafeHwnd()))
        Invalidate(); 
}

BOOL CMapHyperLink::GetVisited() const
{ 
    return m_bVisited; 
}

void CMapHyperLink::SetLinkCursor(HCURSOR hCursor)
{ 
    m_hLinkCursor = hCursor;
    if (m_hLinkCursor == NULL)
        SetDefaultCursor();
}

HCURSOR CMapHyperLink::GetLinkCursor() const
{
    return m_hLinkCursor;
}

void CMapHyperLink::SetAutoSize(BOOL bAutoSize /* = TRUE */)
{
    m_bAdjustToFit = bAutoSize;

    if (::IsWindow(GetSafeHwnd()))
		PositionWindow();
}

BOOL CMapHyperLink::GetAutoSize() const
{ 
    return m_bAdjustToFit; 
}


// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.

void CMapHyperLink::PositionWindow()
{
    if (!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit) 
        return;

    // Get the current window position
    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

	CRect rectMap;
    GetClientRect(rectMap);

    // Get the text justification via the window style
    DWORD dwStyle = GetStyle();

    // Recalc the window size and position based on the text justification
    if (dwStyle & SS_CENTERIMAGE)
        rect.DeflateRect(0, (rect.Height() - rectMap.Height())/2);
    else
        rect.bottom = rect.top + rectMap.Height();

    if (dwStyle & SS_CENTER)   
        rect.DeflateRect((rect.Width() - rectMap.Width())/2, 0);
    else if (dwStyle & SS_RIGHT) 
        rect.left  = rect.right - rectMap.Width();
    else // SS_LEFT = 0, so we can't test for it explicitly 
        rect.right = rect.left + rectMap.Width();

    // Move the window
    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
// CMapHyperLink implementation

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CMapHyperLink::SetDefaultCursor()
{
    if (m_hLinkCursor == NULL)                // No cursor handle - load our own
    {
        // Get the windows directory
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _T("\\winhlp32.exe");
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
                m_hLinkCursor = CopyCursor(hHandCursor);
        }
        FreeLibrary(hModule);
    }
}

LONG CMapHyperLink::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}

void CMapHyperLink::ReportError(int nError)
{
    CString str;
    switch (nError) {
        case 0:                       str = "The operating system is out\nof memory or resources."; break;
        case SE_ERR_PNF:              str = "The specified path was not found."; break;
        case SE_ERR_FNF:              str = "The specified file was not found."; break;
        case ERROR_BAD_FORMAT:        str = "The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image)."; break;
        case SE_ERR_ACCESSDENIED:     str = "The operating system denied\naccess to the specified file."; break;
        case SE_ERR_ASSOCINCOMPLETE:  str = "The filename association is\nincomplete or invalid."; break;
        case SE_ERR_DDEBUSY:          str = "The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed."; break;
        case SE_ERR_DDEFAIL:          str = "The DDE transaction failed."; break;
        case SE_ERR_DDETIMEOUT:       str = "The DDE transaction could not\nbe completed because the request timed out."; break;
        case SE_ERR_DLLNOTFOUND:      str = "The specified dynamic-link library was not found."; break;
        case SE_ERR_NOASSOC:          str = "There is no application associated\nwith the given filename extension."; break;
        case SE_ERR_OOM:              str = "There was not enough memory to complete the operation."; break;
        case SE_ERR_SHARE:            str = "A sharing violation occurred. ";
        default:                      str.Format(_T("Unknown Error (%d) occurred."), nError); break;
    }
    str = "Unable to open hyperlink:\n\n" + str;
    AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
}

HINSTANCE CMapHyperLink::GotoURL(LPCTSTR url, int showcmd)
{
    TCHAR key[MAX_PATH + MAX_PATH];

    // First try ShellExecute()
    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

    // If it failed, get the .htm regkey and lookup the program
    if ((UINT)result <= HINSTANCE_ERROR) {

        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL) {                     // No quotes found
                    pos = _tcsstr(key, _T("%1"));	   // Check for %1, without quotes 
                    if (pos == NULL)                   // No parameter at all...
                        pos = key+lstrlen(key)-1;
                    else
                        *pos = '\0';                   // Remove the parameter
                }
                else
                    *pos = '\0';                       // Remove the parameter

                lstrcat(pos, _T(" "));
                lstrcat(pos, url);
                result = (HINSTANCE) WinExec(TCHAR2char(key),showcmd);
            }
        }
    }

    return result;
}
