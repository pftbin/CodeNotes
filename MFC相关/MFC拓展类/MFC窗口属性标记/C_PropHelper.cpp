#include "StdAfx.h"
#include "C_PropHelper.h"

C_PropHelper::C_PropHelper(HWND hWnd, LPCTSTR lpString)
: m_hWnd(hWnd)
, m_strText(lpString)
{
	this->SetProp(m_hWnd, m_strText);
}

C_PropHelper::~C_PropHelper(void)
{
	this->RemoveProp(m_hWnd, m_strText);
}

void C_PropHelper::SetProp(HWND hWnd, LPCTSTR lpString)
{
	::SetProp(hWnd, lpString, (HANDLE)1);
}

void C_PropHelper::RemoveProp(HWND hWnd, LPCTSTR lpString)
{
	::RemoveProp(hWnd, lpString);
}

HWND C_PropHelper::GetWindow(LPCTSTR lpString)
{
	HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
	while (::IsWindow(hWndPrevious))
	{
		if (::GetProp(hWndPrevious, lpString))
		{
			return hWndPrevious;
		}

		hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
	}

	return NULL;
}
