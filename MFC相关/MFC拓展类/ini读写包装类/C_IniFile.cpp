#include "pch.h"
#include "C_IniFile.h"
 
#define _CRT_SECURE_NO_WARNINGS
C_IniFile::C_IniFile()
{
	memset(m_szFileName, 0x00, MAX_PATH);
}
 
//初始化INI
C_IniFile::C_IniFile(LPCTSTR szFileName)
{
	SetFileName(szFileName);
}
 
C_IniFile::~C_IniFile()
{
 
}
 
void C_IniFile::SetFileName(LPCTSTR szFileName)
{
	memset(m_szFileName, 0x00, MAX_PATH);
	_tcscpy_s(m_szFileName, MAX_PATH, szFileName);
}
 
void C_IniFile::WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int nValue)
{
	TCHAR szValue[MAX_PATH];
	_stprintf_s(szValue, _T("%d"), nValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
 
void C_IniFile::WriteFloat(LPCTSTR szSection, LPCTSTR szKey, float fValue)
{
	TCHAR szValue[MAX_PATH];
	_stprintf_s(szValue, _T("%f"), fValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
 
void C_IniFile::WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, BOOL bValue)
{
	TCHAR szValue[MAX_PATH];
	_stprintf_s(szValue, _T("%s"), bValue ? _T("True") : _T("False"));
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
 
void C_IniFile::WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue)
{
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
 
int C_IniFile::ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int nDefaultValue)
{
	int nResult = GetPrivateProfileInt(szSection, szKey, nDefaultValue, m_szFileName);
	return nResult;
}
 
float C_IniFile::ReadFloat(LPCTSTR szSection, LPCTSTR szKey, float fDefaultValue)
{
	TCHAR szResult[MAX_PATH];
	TCHAR szDefault[MAX_PATH];
	float fResult;
	_stprintf_s(szDefault, _T("%f"), fDefaultValue);
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, MAX_PATH, m_szFileName);
	fResult = (float)_tstof(szResult);
	return fResult;
}
 
BOOL C_IniFile::ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, BOOL bDefaultValue)
{
	TCHAR szResult[MAX_PATH];
	TCHAR szDefault[MAX_PATH];
	bool bResult;
	_stprintf_s(szDefault, _T("%s"), bDefaultValue ? _T("TRUE") : _T("FALSE"));
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, MAX_PATH, m_szFileName);
	bResult = (_tcsicmp(szResult, _T("TRUE")) == 0) ? true : false;
	return bResult;
}
 
CString C_IniFile::ReadString(LPCTSTR szSection, LPCTSTR szKey, const LPCTSTR szDefaultValue)
{
	CString strResult;
	TCHAR* szResult = new TCHAR[MAX_PATH];
	memset(szResult, 0x00, MAX_PATH);
	GetPrivateProfileString(szSection, szKey,
		szDefaultValue, szResult, MAX_PATH, m_szFileName);
	strResult = szResult;
	delete[] szResult;
	szResult = NULL;
	return strResult;
}
 
void C_IniFile::DeleteSection(LPCTSTR szSection)
{
	WritePrivateProfileString(szSection, NULL, NULL, m_szFileName);
}
 
void C_IniFile::DeleteKey(LPCTSTR szSection, LPCTSTR szKey)
{
	WritePrivateProfileString(szSection, szKey, NULL, m_szFileName);
}