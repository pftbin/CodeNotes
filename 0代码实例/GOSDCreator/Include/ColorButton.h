//***************************************************************************
//
// AUTHOR:  James White (feel free to remove or otherwise mangle any part)
//
// DESCRIPTION: This class is alarmingly similar to the CColourPicker control
//	created by Chris Maunder of www.codeproject.com. It is so as it was blatantly
//	copied from that class and is entirely dependant on his other great work
//  in CColourPopup. I was hoping for (cough.. gag..) a more Microsoft look
//  and I think this is pretty close. Hope you like it.
//
// ORIGINAL: http://www.codeproject.com/miscctrl/colour_picker.asp
//
//***************************************************************************
#ifndef COLORBUTTON_INCLUDED
#define COLORBUTTON_INCLUDED
#pragma once

#ifndef COLOURPOPUP_INCLUDED
//#include "WaveAutoSegment\Tools\BtnST.h"
#endif//COLOURPOPUP_INCLUDED

// Return values
#ifndef	BTNST_OK
#define	BTNST_OK						0
#endif
#ifndef	BTNST_INVALIDRESOURCE
#define	BTNST_INVALIDRESOURCE			1
#endif
#ifndef	BTNST_FAILEDMASK
#define	BTNST_FAILEDMASK				2
#endif
#ifndef	BTNST_INVALIDINDEX
#define	BTNST_INVALIDINDEX				3
#endif
#ifndef	BTNST_INVALIDALIGN
#define	BTNST_INVALIDALIGN				4
#endif
#ifndef	BTNST_BADPARAM
#define	BTNST_BADPARAM					5
#endif
#ifndef	BTNST_INVALIDPRESSEDSTYLE
#define	BTNST_INVALIDPRESSEDSTYLE		6
#endif

// Dummy identifier for grayscale icon
#ifndef	BTNST_AUTO_GRAY
#define	BTNST_AUTO_GRAY					(HICON)(0xffffffff - 1L)
#endif
// Dummy identifier for 15% darker icon
#ifndef	BTNST_AUTO_DARKER
#define	BTNST_AUTO_DARKER				(HICON)(0xffffffff - 2L)
#endif

//messages
#define CPN_SELCHANGE        WM_USER + 1001        // Colour Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Colour Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Colour Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Colour Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Colour Picker end (cancelled)


void AFXAPI DDX_ColorButton(CDataExchange *pDX, int nIDC, COLORREF& crColour);

class CColorButton : public CButton
{
public:
	DECLARE_DYNCREATE(CColorButton);

	//***********************************************************************
	// Name:		CColorButton
	// Description:	Default constructor.
	// Parameters:	None.
	// Return:		None.	
	// Notes:		None.
	//***********************************************************************
	CColorButton(void);

	//***********************************************************************
	// Name:		CColorButton
	// Description:	Destructor.
	// Parameters:	None.
	// Return:		None.		
	// Notes:		None.
	//***********************************************************************
	virtual ~CColorButton(void);


	enum	{	ST_ALIGN_HORIZ	= 0,			// Icon/bitmap on the left, text on the right
		ST_ALIGN_VERT,					// Icon/bitmap on the top, text on the bottom
		ST_ALIGN_HORIZ_RIGHT,			// Icon/bitmap on the right, text on the left
		ST_ALIGN_OVERLAP				// Icon/bitmap on the same space as text
	};

	enum	{	BTNST_COLOR_BK_IN	= 0,		// Background color when mouse is INside
		BTNST_COLOR_FG_IN,				// Text color when mouse is INside
		BTNST_COLOR_BK_OUT,				// Background color when mouse is OUTside
		BTNST_COLOR_FG_OUT,				// Text color when mouse is OUTside
		BTNST_COLOR_BK_FOCUS,			// Background color when the button is focused
		BTNST_COLOR_FG_FOCUS,			// Text color when the button is focused

		BTNST_MAX_COLORS
	};

	enum	{	BTNST_PRESSED_LEFTRIGHT = 0,	// Pressed style from left to right (as usual)
		BTNST_PRESSED_TOPBOTTOM			// Pressed style from top to bottom
	};




	//***********************************************************************
	//**                        Property Accessors                         **
	//***********************************************************************	
	__declspec(property(get=GetColor,put=SetColor))						COLORREF	Color;
	__declspec(property(get=GetDefaultColor,put=SetDefaultColor))		COLORREF	DefaultColor;
	__declspec(property(get=GetTrackSelection,put=SetTrackSelection))	BOOL		TrackSelection;
	__declspec(property(put=SetCustomText))								LPCTSTR		CustomText;
	__declspec(property(put=SetDefaultText))							LPCTSTR		DefaultText;

	//***********************************************************************
	// Name:		GetColor
	// Description:	Returns the current color selected in the control.
	// Parameters:	void
	// Return:		COLORREF 
	// Notes:		None.
	//***********************************************************************
	COLORREF GetColor(void) const;

	//***********************************************************************
	// Name:		SetColor
	// Description:	Sets the current color selected in the control.
	// Parameters:	COLORREF Color
	// Return:		None. 
	// Notes:		None.
	//***********************************************************************
	void SetColor(COLORREF Color);


	//***********************************************************************
	// Name:		GetDefaultColor
	// Description:	Returns the color associated with the 'default' selection.
	// Parameters:	void
	// Return:		COLORREF 
	// Notes:		None.
	//***********************************************************************
	COLORREF GetDefaultColor(void) const;

	//***********************************************************************
	// Name:		SetDefaultColor
	// Description:	Sets the color associated with the 'default' selection.
	//				The default value is COLOR_APPWORKSPACE.
	// Parameters:	COLORREF Color
	// Return:		None. 
	// Notes:		None.
	//***********************************************************************
	void SetDefaultColor(COLORREF Color);

	//***********************************************************************
	// Name:		SetCustomText
	// Description:	Sets the text to display in the 'Custom' selection of the
	//				CColourPicker control, the default text is "More Colors...".
	// Parameters:	LPCTSTR tszText
	// Return:		None. 
	// Notes:		None.
	//***********************************************************************
	void SetCustomText(LPCTSTR tszText);

	//***********************************************************************
	// Name:		SetDefaultText
	// Description:	Sets the text to display in the 'Default' selection of the
	//				CColourPicker control, the default text is "Automatic". If
	//				this value is set to "", the 'Default' selection will not
	//				be shown.
	// Parameters:	LPCTSTR tszText
	// Return:		None. 
	// Notes:		None.
	//***********************************************************************
	void SetDefaultText(LPCTSTR tszText);

	//***********************************************************************
	// Name:		SetTrackSelection
	// Description:	Turns on/off the 'Track Selection' option of the control
	//				which shows the colors during the process of selection.
	// Parameters:	BOOL bTrack
	// Return:		None. 
	// Notes:		None.
	//***********************************************************************
	void SetTrackSelection(BOOL bTrack);

	//***********************************************************************
	// Name:		GetTrackSelection
	// Description:	Returns the state of the 'Track Selection' option.
	// Parameters:	void
	// Return:		BOOL 
	// Notes:		None.
	//***********************************************************************
	BOOL GetTrackSelection(void) const;
	//add function
	DWORD OnDrawBorder(CDC* pDC, CRect* pRect);
	DWORD SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut = NULL, COLORREF crTransColorOut = 0);
	DWORD SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut = NULL, COLORREF crTransColorOut = 0);
	void PrepareImageRect(BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* rpImage);
	HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor);
	//void DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
	void DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled);
	void FreeResources(BOOL bCheckForNULL = TRUE);
	DWORD SetPressedStyle(BYTE byStyle, BOOL bRepaint = TRUE);
	//{{AFX_VIRTUAL(CColorButton)

	void SelectColor();
	void SetSelected(bool bSel);
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CColorButton)
    afx_msg BOOL OnDbClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);		

	//***********************************************************************
	// Name:		DrawArrow
	// Description:	None.
	// Parameters:	CDC* pDC
	//				RECT* pRect
	//				int iDirection
	//					0 - Down
	//					1 - Up
	//					2 - Left
	//					3 - Right
	// Return:		static None. 
	// Notes:		None.
	//***********************************************************************
	static void DrawArrow(CDC* pDC, 
						  RECT* pRect, 
						  int iDirection = 0,
						  COLORREF clrArrow = RGB(0,0,0));


	DECLARE_MESSAGE_MAP()

	COLORREF m_Color;
	COLORREF m_DefaultColor;
	CString m_strDefaultText;
	CString m_strCustomText;
	BOOL	m_bPopupActive;
	BOOL	m_bTrackSelection;
	BOOL		m_bIsFlat;			// Is a flat button?
	BOOL		m_bMouseOnButton;	// Is mouse over the button?
	BOOL		m_bDrawTransparent;	// Draw transparent?
	BOOL		m_bIsPressed;		// Is button pressed?
	BOOL		m_bIsFocused;		// Is button focused?
	BOOL		m_bIsDisabled;		// Is button disabled?
	BOOL		m_bIsDefault;		// Is default button?
	BOOL		m_bIsCheckBox;		// Is the button a checkbox?
	BYTE		m_byAlign;			// Align mode
	BOOL		m_bDrawBorder;		// Draw border?
	BOOL		m_bDrawFlatFocus;	// Draw focus rectangle for flat button?
	BOOL		m_bAlwaysTrack;		// Always hilight button?
	BOOL	m_bShowDisabledBitmap;
	POINT	m_ptImageOrg;
	POINT	m_ptPressedOffset;

	bool		m_bSelected;

private:
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	typedef CButton _Inherited;


#pragma pack(1)
	typedef struct _STRUCT_BITMAPS
	{
		HBITMAP		hBitmap;		// Handle to bitmap
		DWORD		dwWidth;		// Width of bitmap
		DWORD		dwHeight;		// Height of bitmap
		HBITMAP		hMask;			// Handle to mask bitmap
		COLORREF	crTransparent;	// Transparent color
	} STRUCT_BITMAPS;

STRUCT_BITMAPS	m_csBitmaps[2];
#pragma pack()


};

#endif //!COLORBUTTON_INCLUDED

