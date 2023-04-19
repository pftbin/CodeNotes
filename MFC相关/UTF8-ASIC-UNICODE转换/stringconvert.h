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


static void unicode_to_utf8(const wchar_t* in, size_t len, std::string& out)
{
#ifdef WIN32
    int out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    if (out_len > 0) {
        char* lpBuf = new char[static_cast<unsigned int>(out_len + 1)];
        if (lpBuf) {
            memset(lpBuf, 0, static_cast<unsigned int>(out_len + 1));
            int return_len = ::WideCharToMultiByte(CP_UTF8, 0, in, static_cast<int>(len), lpBuf, out_len, nullptr, nullptr);
            if (return_len > 0) out.assign(lpBuf, static_cast<unsigned int>(return_len));
            delete[]lpBuf;
        }
    }
#else
    /*if (wcslen(in) <= 0 || len <= 0) return;
    std::lock_guard<std::mutex> guard(g_ConvertCodeMutex);
    size_t w_len = len * 4 + 1;
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<char[]> p(new char[w_len]);
    size_t return_len = wcstombs(p.get(), in, w_len);
    if (return_len > 0) out.assign(p.get(), return_len);
    setlocale(LC_CTYPE, "C");*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char)) + 1U;
    char* save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-32", "UTF-8//IGNORE", save, w_len, (char*)(in), w_len);
    out = save;
    delete[]save;
#endif
}

static void utf8_to_unicode(const char* in, size_t len, std::wstring& out)
{
#ifdef WIN32
    int out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, static_cast<int>(len), nullptr, 0);
    if (out_len > 0) {
        wchar_t* lpBuf = new wchar_t[static_cast<unsigned int>(out_len + 1)];
        if (lpBuf) {
            memset(lpBuf, 0, (static_cast<unsigned int>(out_len + 1)) * sizeof(wchar_t));
            int return_len = ::MultiByteToWideChar(CP_UTF8, 0, in, static_cast<int>(len), lpBuf, out_len);
            if (return_len > 0) out.assign(lpBuf, static_cast<unsigned int>(return_len));
            delete[]lpBuf;
        }
    }
#else
    /*if (strlen(in) <= 0 || len <= 0) return;
    std::lock_guard<std::mutex> guard(g_ConvertCodeMutex);
    setlocale(LC_CTYPE, "en_US.UTF-8");
    std::unique_ptr<wchar_t[]> p(new wchar_t[sizeof(wchar_t) * (len + 1)]);
    size_t return_len = mbstowcs(p.get(), in, len + 1);
    if (return_len > 0) out.assign(p.get(), return_len);
    setlocale(LC_CTYPE, "C");*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char)) + 1U;
    char* save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-8", "UTF-32//IGNORE", save, w_len, (char*)(in), len);
    out = (wchar_t*)save + 1;
    delete[]save;
#endif
}

static void ansi_to_unicode(const char* in, size_t len, std::wstring& out)
{
#ifdef WIN32
    int out_len = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, static_cast<int>(len), nullptr, 0);
    if (out_len > 0) {
        wchar_t* lpBuf = new wchar_t[static_cast<unsigned int>(out_len + 1)];
        if (lpBuf) {
            memset(lpBuf, 0, (static_cast<unsigned int>(out_len + 1)) * sizeof(wchar_t));
            int return_len = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, static_cast<int>(len), lpBuf, out_len);
            if (return_len > 0) out.assign(lpBuf, static_cast<unsigned int>(return_len));
            delete[]lpBuf;
        }
    }
#else
    /*mbstate_t state;
    memset (&state, '\0', sizeof (state));
    std::lock_guard<std::mutex> guard(g_ConvertCodeMutex);
    //setlocale(LC_CTYPE, "zh_CN.UTF-8");
    setlocale(LC_CTYPE, "en_US.UTF-8");
    size_t out_len= mbsrtowcs(nullptr, &in, 0, &state);
    if (out_len > 0 &&  out_len < UINT_MAX && len > 0)
    {
        std::unique_ptr<wchar_t[]> lpBuf(new wchar_t[sizeof(wchar_t) * (out_len + 1)]);
        size_t return_len = mbsrtowcs(lpBuf.get(), &in, out_len+1, &state);
        if (return_len > 0) out.assign(lpBuf.get(), return_len);
    }
    setlocale(LC_CTYPE, "C");*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char)) + 1U;
    char* save = new char[w_len];
    memset(save, '\0', w_len);

    iconv_convert("GBK", "UTF-32//IGNORE", save, w_len, (char*)(in), len);
    out = (wchar_t*)save + 1;
    delete[]save;
#endif
}

static void unicode_to_ansi(const wchar_t* in, size_t len, std::string& out)
{
#ifdef WIN32
    int out_len = ::WideCharToMultiByte(CP_ACP, 0, in, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    if (out_len > 0) {
        char* lpBuf = new char[static_cast<unsigned int>(out_len + 1)];
        if (lpBuf) {
            memset(lpBuf, 0, static_cast<unsigned int>(out_len + 1));
            int return_len = ::WideCharToMultiByte(CP_ACP, 0, in, static_cast<int>(len), lpBuf, out_len, nullptr, nullptr);
            if (return_len > 0) out.assign(lpBuf, static_cast<unsigned int>(return_len));
            delete[]lpBuf;
        }
    }
#else
    /*mbstate_t state;
    memset(&state, '\0', sizeof(state));
    std::lock_guard<std::mutex> guard(g_ConvertCodeMutex);
    setlocale(LC_CTYPE, "en_US.UTF-8");
    size_t out_len = wcsrtombs(nullptr, &in, 0, &state);
    if (out_len > 0 && out_len < UINT_MAX && len > 0)
    {
        std::unique_ptr<char[]> lpBuf(new char[sizeof(char) *(out_len + 1)]);
        size_t return_len = wcsrtombs(lpBuf.get(), &in, out_len+1, &state);//wcstombs(lpBuf.get(), in, len + 1);
        if (return_len > 0) out.assign(lpBuf.get(), return_len);
    }
    setlocale(LC_CTYPE, "C");*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char)) + 1U;
    char* save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-32", "GBK//IGNORE", save, w_len, (char*)(in), w_len - 1U);
    out = save;
    delete[]save;
#endif
}

static void ansi_to_utf8(const char* in, size_t len, std::string& out)
{
#ifdef WIN32
    std::wstring strUnicode;
    ansi_to_unicode(in, len, strUnicode);
    return unicode_to_utf8(strUnicode.c_str(), strUnicode.length(), out);
#else
    /*std::wstring strUnicode;
    ansi_to_unicode(in, len, strUnicode);
    return unicode_to_utf8(strUnicode.c_str(), strUnicode.length(), out);*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char));
    char* save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("GBK", "UTF-8//IGNORE", save, w_len, (char*)(in), len);
    out = save;
    delete[]save;
#endif
}

static void utf8_to_ansi(const char* in, size_t len, std::string& out)
{
#ifdef WIN32
    std::wstring strUnicode;
    utf8_to_unicode(in, len, strUnicode);
    return unicode_to_ansi(strUnicode.c_str(), strUnicode.length(), out);
#else
    /*std::wstring strUnicode;
    utf8_to_unicode(in, len, strUnicode);
    return unicode_to_ansi(strUnicode.c_str(), strUnicode.length(), out);*/

    size_t w_len = len * (sizeof(wchar_t) / sizeof(char));
    char* save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-8", "GBK//IGNORE", save, w_len, (char*)(in), len);
    out = save;
    delete[]save;
#endif
}

//
static int globalStrToIntDef(const LPTSTR valuechar, int& value, int defaultvalue = -1, int base = 10)
{
    if (base < 2 || base > 16) base = 10;
    value = defaultvalue;
#if defined(UNICODE) || defined(_UNICODE)
    wchar_t* endptr;
    errno = 0;
    long val = wcstol(valuechar, &endptr, base);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        return -1;
    }

    if (endptr == valuechar)//没有找到有效的字符
    {
        return -1;
    }
    if (*endptr != '\0')
    {
        //value = (int)val;
        //return 0;
        return -1;
    }
#else
    char* endptr;
    errno = 0;
    long val = strtol(valuechar, &endptr, base);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        return -1;
    }

    if (endptr == const_cast<char*>(valuechar))//(char*)valuechar)//没有找到有效的字符
    {
        return -1;
    }
    if (*endptr != '\0')
    {
        //value = (int)val;
        //return 0;
        return -1;
    }
#endif
    value = static_cast<int>(val);//(int)val;
    return 1;
}

static int globalStrToInt(const LPTSTR valuechar, int defaultvalue, int base)
{
    if (base < 2 || base > 16) base = 10;
#if defined(UNICODE) || defined(_UNICODE)
    wchar_t* endptr;
    errno = 0;
    long val = wcstol(valuechar, &endptr, base);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        return defaultvalue;
    }

    if (endptr == valuechar)//没有找到有效的字符
    {
        return defaultvalue;
    }
    if (*endptr != '\0')
    {
        return defaultvalue;
    }
#else
    char* endptr;
    errno = 0;
    long val = strtol(valuechar, &endptr, base);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        return defaultvalue;
    }

    if (endptr == const_cast<char*>(valuechar))//(char*)valuechar)//没有找到有效的字符
    {
        return defaultvalue;
    }
    if (*endptr != '\0')
    {
        return -1;
    }
#endif
    return static_cast<int>(val);
}

static void globalSpliteString(const std::tstring& str, std::vector<std::tstring>& strVector, std::tstring splitStr, int maxCount = 1024)
{
    std::tstring::size_type pos = 0U;
    std::tstring::size_type pre = 0U;
    std::tstring::size_type len = str.length();
    if (splitStr.empty()) splitStr = _T("|");
    std::tstring::size_type splitLen = splitStr.length();
    std::tstring curStr;
    if (maxCount < 1) maxCount = 1;
    do {
        if (static_cast<int>(strVector.size()) >= maxCount - 1) {//如果超过最大个数则不解析最后的分隔字符串，直接将其放在最后一个里面
            curStr = str.substr(pre, len - pre);
            strVector.push_back(curStr);
            break;
        }
        pos = str.find(splitStr, pre);
        if (pos == 0U) {
            pre = pos + splitLen;
            continue;
        }
        else if (pos == std::tstring::npos) {
            curStr = str.substr(pre, len - pre);
        }
        else {
            curStr = str.substr(pre, pos - pre);
            pre = pos + splitLen;
        }
        strVector.push_back(curStr);
    } while (pos != std::tstring::npos && pos != (len - splitLen));
}

static bool globalIsIPStringValid(std::tstring IPString)
{
    std::string strIpAddr;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(IPString.c_str(), IPString.length(), strIpAddr);
#else
    strIpAddr = IPString;
#endif
    /*std::string::size_type nbytes = strIpAddr.length();
    int num = 0;
    for (std::string::size_type i = 0; i < nbytes; i++)
    {
        if (strIpAddr.at(i) == '.') num++;
    }
    if (num != 3)
    {
        return false;
    }*/
    std::vector<std::tstring> ipVector;
    globalSpliteString(IPString, ipVector, _T("."));
    if (ipVector.size() != 4U) return false;
    for (std::vector<std::tstring>::iterator it = ipVector.begin(); it != ipVector.end(); it++)
    {
        int temp;
        globalStrToIntDef(const_cast<LPTSTR>(it->c_str()), temp);
        if (temp < 0 || temp > 255) return false;
    }
    struct in_addr addr;
    //if (inet_pton(AF_INET, strIpAddr.c_str(), (void *)&addr) == 1)
    if (inet_pton(AF_INET, strIpAddr.c_str(), reinterpret_cast<void*>(&addr)) == 1)
        return true;
    else
        return false;
}



