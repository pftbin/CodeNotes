#pragma once
#include <iostream>
#include <vector>
#include <clocale>
#include <cwchar>
#include <codecvt>
#include <string.h>

#ifdef UNICODE
#define tstring wstring
#else
#define tstring string
#endif
using namespace std;


#if defined(UNICODE) || defined(_UNICODE)
#include <windows.h>

//1: Unicode <=> Ansi
std::wstring AnsiToUnicode(const std::string& strAnsi)
{
	//获取转换所需的接收缓冲区大小
	int  nUnicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		strAnsi.c_str(),
		-1,
		NULL,
		0);
	//分配指定大小的内存
	wchar_t* pUnicode = new wchar_t[nUnicodeLen + 1];
	memset((void*)pUnicode, 0, (nUnicodeLen + 1) * sizeof(wchar_t));

	//转换
	::MultiByteToWideChar(CP_ACP,
		0,
		strAnsi.c_str(),
		-1,
		(LPWSTR)pUnicode,
		nUnicodeLen);

	std::wstring  strUnicode;
	strUnicode = (wchar_t*)pUnicode;
	delete[]pUnicode;

	return strUnicode;
}
std::string UnicodeToAnsi(const std::wstring& strUnicode)
{
	int nAnsiLen = WideCharToMultiByte(CP_ACP,
		0,
		strUnicode.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);

	char* pAnsi = new char[nAnsiLen + 1];
	memset((void*)pAnsi, 0, (nAnsiLen + 1) * sizeof(char));

	::WideCharToMultiByte(CP_ACP,
		0,
		strUnicode.c_str(),
		-1,
		pAnsi,
		nAnsiLen,
		NULL,
		NULL);

	std::string strAnsi;
	strAnsi = pAnsi;
	delete[]pAnsi;

	return strAnsi;
}


//2: Unicode <=> Utf8
std::wstring Utf8ToUnicode(const std::string& str)
{
	int nUnicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);

	wchar_t* pUnicode;
	pUnicode = new wchar_t[nUnicodeLen + 1];
	memset((void*)pUnicode, 0, (nUnicodeLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		nUnicodeLen);

	std::wstring  strUnicode;
	strUnicode = (wchar_t*)pUnicode;
	delete[]pUnicode;

	return strUnicode;
}
std::string UnicodeToUtf8(const std::wstring& strUnicode)
{
	int nUtf8Length = WideCharToMultiByte(CP_UTF8,
		0,
		strUnicode.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);

	char* pUtf8 = new char[nUtf8Length + 1];
	memset((void*)pUtf8, 0, sizeof(char) * (nUtf8Length + 1));

	::WideCharToMultiByte(CP_UTF8,
		0,
		strUnicode.c_str(),
		-1,
		pUtf8,
		nUtf8Length,
		NULL,
		NULL);

	std::string strUtf8;
	strUtf8 = pUtf8;
	delete[] pUtf8;

	return strUtf8;
}


#else

//1: Unicode <=> Ansi
std::string UnicodeToAnsi(const std::wstring& wstr)
{
	std::string ret;
	std::mbstate_t state = {};
	const wchar_t* src = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< char[] > buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}
std::wstring AnsiToUnicode(const std::string& str)
{
	std::wstring ret;
	std::mbstate_t state = {};
	const char* src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< wchar_t[] > buff(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	return ret;
}


//2: Unicode <=> Utf8
std::string UnicodeToUtf8(const std::wstring& wstr)
{
	std::string ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(wstr);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}

std::wstring Utf8ToUnicode(const std::string& str)
{
	std::wstring ret;
	try {
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(str);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}


#endif


//=====================================================
std::string AnsiToUtf8(const std::string& strAnsi)
{
    std::wstring strUnicode = AnsiToUnicode(strAnsi);
    return UnicodeToUtf8(strUnicode);
}

std::string Utf8ToAnsi(const std::string& strUtf8)
{
    std::wstring strUnicode = Utf8ToUnicode(strUtf8);
    return UnicodeToAnsi(strUnicode);
}






