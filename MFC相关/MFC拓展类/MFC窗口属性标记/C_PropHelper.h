#pragma once

class C_PropHelper
{
public:
	C_PropHelper(HWND hWnd, LPCTSTR lpString);
	~C_PropHelper(void);

public:
	static void SetProp(HWND hWnd, LPCTSTR lpString);
	static void RemoveProp(HWND hWnd, LPCTSTR lpString);
	static HWND GetWindow(LPCTSTR lpString);

protected:
	HWND m_hWnd;
	CString m_strText;
};
