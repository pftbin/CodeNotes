#include "public.h"
#include <algorithm>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/filesystem.hpp>

#ifdef WIN32
#include <WS2tcpip.h>
#include <TlHelp32.h>
#include <objbase.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <signal.h>
//#include <iconv.h>//zwb修改，因为有的系统下有多个该文件，导致系统编译不过
#include </usr/include/iconv.h>
#include <string.h>

#define _TRUNCATE ((size_t)-1)

#define _snprintf_s(a,b,c,...) snprintf(a,b,__VA_ARGS__)
#define _sntprintf_s    _snprintf_s

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define lstrlen  strlen

#endif // !WIN32

//计算机名
TCHAR           g_ComputerName[MAX_PATH];//计算机名
std::tstring    g_ComputerNameStr;//计算机名
std::tstring    g_ComputerNameUpper;//大写计算机名

/*std::tstring convertToString(int value)
{
	std::tstringstream ss;
	ss << value;
	return ss.str();
}

std::tstring convertToString(int value, int width)
{
	std::tstringstream ss;
#if defined(UNICODE) || defined(_UNICODE)
	ss << std::setw(width) << std::setfill(L'0') << value;
#else
	ss << std::setw(width) << std::setfill('0') << value;
#endif
	return ss.str();
}*/

#ifndef WIN32
std::mutex g_ConvertCodeMutex;  //用于保护字符转换的互斥量
int iconv_convert(const char *from, const char *to, char* save, size_t savelen, char *src, size_t srclen)
{
    iconv_t cd;
    char   *inbuf = src;
    char *outbuf = save;
    size_t outbufsize = savelen;
    //size_t  savesize = 0;// ghq note
    size_t inbufsize = srclen;
    const char* inptr = inbuf;
    size_t      insize = inbufsize;
    char* outptr = outbuf;
    size_t outsize = outbufsize;

    cd = iconv_open(to, from);
    //iconv(cd, NULL, NULL, NULL, NULL);// ghq note
    if (inbufsize == 0U) {
        iconv_close(cd);
        return -1;
    }
    while (insize > 0U) {
        size_t res = iconv(cd, (char**)&inptr, &insize, &outptr, &outsize);
//        if (outptr != outbuf) {
//            int saved_errno = errno;
//            int outsize = outptr - outbuf;
//            strncpy(save+savesize, outbuf, outsize);
//            errno = saved_errno;
//        }// ghq note
        if (res == (size_t)(-1)) {
            if (errno == EILSEQ) {
                //int one = 1;
                //iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, &one);
                iconv_close(cd);
                return -3;
            } else if (errno == EINVAL) {
                if (insize == 0U) {//if (inbufsize == 0U) {
                    iconv_close(cd);
                    return -4;
                } else {
                    break;
                }
            } else if (errno == E2BIG) {
                iconv_close(cd);
                return -5;
            } else {
                iconv_close(cd);
                return -6;
            }
        }
    }
    iconv_close(cd);
    size_t activeLen = strlen(save);
    return static_cast<int>(activeLen);
}
#endif

bool globalGetParamFromConfigJson(std::tstring Path, std::map<std::tstring, std::tstring> &lst)
{
    lst.clear();
    if (!globalFileExists(Path)) return false;
	bool successFlag = false;
    try {
        std::ifstream is;
        is.open(Path.c_str(), std::ios::binary);
        std::ostringstream os_stream;
        os_stream << is.rdbuf();
        std::string jsonStr = os_stream.str();
#ifdef WIN32
        utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
        is.close();

        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonStr, root);

        Json::Value::Members members = root.getMemberNames();
        for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
        {
            if (root[*it].isString()) lst[*it] = root[*it].asString();
            else if (root[*it].isInt()) lst[*it] = convertToString(root[*it].asInt());
        }
        successFlag = true;
    } catch (...) {
		;
	}
	return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, std::tstring Value)
{
	bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
		;
	}
	return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, int Value)
{
    bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
        ;
    }
    return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, UINT Value)
{
    bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
        ;
    }
    return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, INT64 Value)
{
    bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = static_cast<Json::Int64>(Value);
        //root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
        ;
    }
    return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, double Value)
{
    bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
        ;
    }
    return successFlag;
}

bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, Json::Value &Value)
{
    bool successFlag = false;
    try {
        Json::Value root;

        if (globalFileExists(Path)) {
            std::ifstream is;
            is.open(Path.c_str(), std::ios::binary);
            std::ostringstream os_stream;
            os_stream << is.rdbuf();
            std::string jsonStr = os_stream.str();
#ifdef WIN32
            utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
            is.close();
            Json::Reader reader;
            reader.parse(jsonStr, root);
        }
        root[Name] = Value;

        std::string WriteStr = root.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif
        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
        ;
    }
    return successFlag;
}

bool globalDeleteParamFromConfigJson(std::tstring Path, std::tstring Name)
{
    if (!globalFileExists(Path)) return false;
	bool successFlag = false;
    try {
        std::ifstream is;
        is.open(Path.c_str(), std::ios::binary);
        std::ostringstream os_stream;
        os_stream << is.rdbuf();
        std::string jsonStr = os_stream.str();
#ifdef WIN32
        utf8_to_ansi(jsonStr.c_str(), jsonStr.length(), jsonStr);//注：文件都是以UTF-8形式存储的，所以要转换，要不中文字符显示有问题
#endif
        is.close();
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonStr, root);

		bool findFlag = false;
		Json::Value::Members members = root.getMemberNames();
		Json::Value root_new;
		for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
		{
			std::string tempName = *it;
            if (tempName == Name) {
				findFlag = true;
            } else {
                root_new[tempName] = root[tempName];
			}
		}

        if (!findFlag) {
			return false;
		}

        std::string WriteStr = root_new.toStyledString();
#ifdef WIN32
        ansi_to_utf8(WriteStr.c_str(), WriteStr.length(), WriteStr);
#endif

        std::ofstream os;
        os.open(Path.c_str(), std::ios::out | std::ios::binary);
        os << WriteStr;
        successFlag = true;
        os.close();
    } catch (...) {
		;
	}
    //新版jsoncpp写法，暂时不用
	/*try
	{
		std::string strPath, strName;
#if defined(UNICODE) || defined(_UNICODE)
        unicode_to_ansi(Path.c_str(), Path.length(), strPath);
        unicode_to_ansi(Name.c_str(), Name.length(), strName);
#else
		strName = Name;
#endif
		std::ifstream is;
		is.open(strPath.c_str(), std::ios::binary);

		Json::CharReaderBuilder rbuilder;
		rbuilder["collectComments"] = false;
		JSONCPP_STRING errs;
		Json::Value root;
		if (!Json::parseFromStream(rbuilder, is, &root, &errs)) return false;
		is.close();

		bool findFlag = false;
		Json::Value::Members members = root.getMemberNames();
		Json::Value root_new;
		for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
		{
			std::string tempName = *it;
			if (tempName == strName)
			{
				findFlag = true;
			}
			else
			{
				root_new[tempName.c_str()] = root[tempName.c_str()];
			}
		}

		if (!findFlag)
		{
			return false;
		}

		std::ofstream os;
		os.open(strPath.c_str(), std::ios::out | std::ios::binary);

		Json::StreamWriterBuilder builder;
		std::unique_ptr<Json::StreamWriter> sw(builder.newStreamWriter());
		sw->write(root, &os);

		successFlag = true;
		os.close();
	}
	catch (...)
	{
		;
	}*/
	return successFlag;
}

void globalGetComputerName()
{
	memset(g_ComputerName, 0, sizeof(TCHAR) * MAX_PATH);
#ifdef WIN32
	DWORD dwLen = MAX_PATH;
	::GetComputerName(g_ComputerName, &dwLen);    
#else
	gethostname(g_ComputerName, sizeof(char) * MAX_PATH);
#endif // WIN32
    g_ComputerNameStr = g_ComputerName;
    g_ComputerNameUpper = upperCase(g_ComputerNameStr);
}

std::tstring upperCase(const std::tstring &in)
{
#ifdef WIN32
	size_t len = in.length();
	if (len < 1) return _T("");
	size_t sLen = (len + 1) * sizeof(TCHAR);
	TCHAR *lpwbuffer = new TCHAR[sLen];
#if defined(UNICODE) || defined(_UNICODE)
	lpwbuffer[len] = L'\0';
	wcscpy_s(lpwbuffer, sLen, in.c_str());
	_wcsupr_s(lpwbuffer, sLen);
#else
	lpwbuffer[len] = '\0';
	strcpy_s(lpwbuffer, sLen, in.c_str());
	_strupr_s(lpwbuffer, sLen);
#endif
	std::tstring out = lpwbuffer;
	delete[]lpwbuffer;
	return out;
#else
    std::tstring resultStr = in;
    std::transform(resultStr.begin(), resultStr.end(), resultStr.begin(), toupper);
	return resultStr;
#endif // WIN32
}

std::tstring lowerCase(const std::tstring &in)
{
#ifdef WIN32
	size_t len = in.length();
	if (len < 1) return _T("");
	size_t sLen = (len + 1) * sizeof(TCHAR);
	TCHAR *lpwbuffer = new TCHAR[sLen];
	lpwbuffer[len] = L'\0';
#if defined(UNICODE) || defined(_UNICODE)
	wcscpy_s(lpwbuffer, sLen, in.c_str());
	_wcslwr_s(lpwbuffer, sLen);
#else
	strcpy_s(lpwbuffer, sLen, in.c_str());
	_strlwr_s(lpwbuffer, sLen);
#endif
	std::tstring out = lpwbuffer;
	delete[]lpwbuffer;
	return out;
#else
    std::tstring resultStr = in;
    std::transform(resultStr.begin(), resultStr.end(), resultStr.begin(), tolower);
	return resultStr;
#endif // WIN32
}

std::tstring replace(const TCHAR *pszSrc, const TCHAR *pszOld, const TCHAR *pszNew)
{
    std::tstring strContent, strTemp;
    strContent.assign( pszSrc );
    std::tstring::size_type nPos = 0U;
    while( true ) {
        nPos = strContent.find(pszOld, nPos);       
        if ( nPos == std::tstring::npos ) break;
        strTemp = strContent.substr(nPos+strlen(pszOld), strContent.length());
        strContent.replace(nPos,strContent.length(), pszNew );
        strContent.append(strTemp);
        nPos += strlen(pszNew) - strlen(pszOld) + 1U; //防止重复替换 避免死循环
    }
    return strContent;
}

std::tstring globalExtractFileName(const std::tstring &Path)
{
	if (Path.empty()) return _T("");
    std::tstring ftpstr = Path.substr(0U, 4U);
	std::tstring splitstr;
    ftpstr = lowerCase(ftpstr);
	if (ftpstr == _T("ftp:"))
	{
		splitstr = _T("/");
	}
	else
	{
		splitstr = PATH_SPLIT;
	}
	std::tstring::size_type Pos = Path.rfind(splitstr);
	if (Pos == std::tstring::npos) return Path;
    else return Path.substr(Pos + 1U);
}

std::tstring globalExtractFileExt(const std::tstring &Path)
{
	if (Path.empty()) return _T("");
	std::tstring splitstr = _T(".");
	std::tstring::size_type Pos = Path.rfind(splitstr);
	if (Pos == std::tstring::npos) return Path;
	else return Path.substr(Pos, Path.length() - Pos);
}

std::tstring globalExtractFilePath(const std::tstring &Path)
{
	if (Path.empty()) return _T("");
    std::tstring ftpstr = Path.substr(0U, 4U);
	std::tstring splitstr;
    ftpstr = lowerCase(ftpstr);
	if (ftpstr == _T("ftp:"))
	{
		splitstr = _T("/");
	}
	else
	{
		splitstr = PATH_SPLIT;
	}
	std::tstring::size_type Pos = Path.rfind(splitstr);
    if (Pos == std::tstring::npos || Pos == (Path.length() - 1U)) return Path;
    else return Path.substr(0U, Pos + 1U);
}

bool globalExtractFatherPath(const std::tstring &Path, std::tstring &FatherPath)
{
	if (Path.empty()) return false;
    std::tstring ftpstr = Path.substr(0U, 4U);
	std::tstring splitstr;
    ftpstr = lowerCase(ftpstr);
	if (ftpstr == _T("ftp:"))
	{
		splitstr = _T("/");
	}
	else
	{
		splitstr = PATH_SPLIT;
	}
	std::tstring::size_type Pos = Path.rfind(splitstr);
	if (Pos == std::tstring::npos) return false;
    FatherPath = Path.substr(0U, Pos);
	Pos = FatherPath.rfind(splitstr);
	if (Pos == std::tstring::npos) return false;
    FatherPath = FatherPath.substr(0U, Pos + 1U);
	return true;
}

std::tstring globalExtractFileNameExcludeExt(const std::tstring &Path)
{
	if (Path.empty()) return _T("");
	std::tstring::size_type Pos1;
    std::tstring ftpstr = Path.substr(0U, 4U);
	std::tstring splitstr;
    ftpstr = lowerCase(ftpstr);
	if (ftpstr == _T("ftp:"))
	{
		splitstr = _T("/");
	}
	else
	{
		splitstr = PATH_SPLIT;
	}
	Pos1 = Path.rfind(splitstr);
    if (Pos1 == std::tstring::npos) Pos1 = 0U;
	else Pos1++;
	std::tstring::size_type Pos2 = Path.rfind(_T("."));
	if (Pos2 == std::tstring::npos) return Path.substr(Pos1, Path.size() - Pos1);
	else return Path.substr(Pos1, Pos2 - Pos1);
}

std::tstring globalChangeFileExt(const std::tstring &Path, const std::tstring &Ext)
{
	if (Path.empty()) return _T("");
	std::tstring::size_type Pos = Path.rfind(_T("."));
	if (Pos == std::tstring::npos) return Path + Ext;
    else return Path.substr(0U, Pos) + Ext;
}

void globalIncludePathDelimiter(std::tstring &Path)
{
	Path = trim(Path);
	if (Path.empty()) return;
	bool FtpFlag = false;
    if (Path.length() > 6U) {
        std::tstring ftpstr = Path.substr(0U, 6U);
        ftpstr = lowerCase(ftpstr);
		if (ftpstr == _T("ftp://")) FtpFlag = true;
	}
    if (FtpFlag) {
		std::tstring::size_type Pos = Path.rfind(_T("/"));
        if (Pos != (Path.length() - 1U)) Path = Path + _T("/");
	}
    else {
#ifdef WIN32
		std::tstring::size_type Pos = Path.rfind(_T("\\"));
		if (Pos != (Path.length() - 1)) Path = Path + _T("\\");
#else
		std::tstring::size_type Pos = Path.rfind(_T("/"));
        if (Pos != (Path.length() - 1U)) Path = Path + _T("/");
#endif // WIN32	
	}
}

void globalIncludeUrlDelimiter(std::tstring &Url)
{
    Url = trim(Url);
    if (Url.empty()) return;
    std::tstring::size_type Pos = Url.rfind(_T("/"));
    if (Pos != (Url.length() - 1U)) Url = Url + _T("/");
}

void globalCorrectPath(std::tstring &Path)
{
    std::tstring::size_type pos = 0U;
    std::tstring ftpstr = Path.substr(0U, 4U);
	std::tstring strSrc, strDes;
	std::tstring::size_type srcLen;
	std::tstring::size_type desLen;

    ftpstr = lowerCase(ftpstr);
    if (ftpstr == _T("ftp:")) {
		strSrc = _T("\\");
		strDes = _T("/");
	}
    else {
#ifdef WIN32
		strSrc = _T("/");
		strDes = _T("\\");
#else
		strSrc = _T("\\");
		strDes = _T("/");
#endif // WIN32	
	}
	srcLen = strSrc.size();
	desLen = strDes.size();
	pos = Path.find(strSrc, pos);
    while ((pos != std::tstring::npos)) {
		Path.replace(pos, srcLen, strDes);
		pos = Path.find(strSrc, (pos + desLen));
	}
}

void globalSpliteString(const std::tstring &str, std::vector<std::tstring> &strVector, std::tstring splitStr, int maxCount)
{
    std::tstring::size_type pos = 0U;
    std::tstring::size_type pre = 0U;
    std::tstring::size_type len = str.length();
    if (splitStr.empty()) splitStr = _T("|");
    std::tstring::size_type splitLen = splitStr.length();
	std::tstring curStr;
    if (maxCount < 1) maxCount = 1;
	do {
        if (static_cast<int>(strVector.size()) >= maxCount -1) {//如果超过最大个数则不解析最后的分隔字符串，直接将其放在最后一个里面
            curStr = str.substr(pre, len - pre);
            strVector.push_back(curStr);
            break;
        }
        pos = str.find(splitStr, pre);
        if (pos == 0U) {
            pre = pos + splitLen;
			continue;
		} else if (pos == std::tstring::npos) {
            curStr = str.substr(pre, len - pre);
		} else {
            curStr = str.substr(pre, pos - pre);
            pre = pos + splitLen;
		}
        strVector.push_back(curStr);
    } while (pos != std::tstring::npos && pos != (len - splitLen));
}

std::tstring globalGetHostByPath(const std::tstring &Path)
{
	if (Path.empty()) return _T("");
	std::tstring::size_type pos = Path.find(_T("\\\\"));
    if (pos == 0) {
		pos = Path.find(_T("\\"), 2);
		if (pos == std::tstring::npos) return Path.substr(2, Path.length() - 2);
		else return Path.substr(2, pos - 2);
    } else {
		return _T("");
	}
}

std::tstring globalGetFilePath()
{
	TCHAR szFileName[MAX_PATH];
	memset(szFileName, 0, MAX_PATH * sizeof(TCHAR));
#ifdef WIN32
    ::GetModuleFileName(::GetModuleHandle(nullptr), szFileName, sizeof(szFileName));
#else
    size_t len = static_cast<size_t>(MAX_PATH);
    long ret_long = readlink("/proc/self/exe", szFileName, len);
    int ret = static_cast<int>(ret_long);
    if (ret < 0 || (ret >= (MAX_PATH - 1))){
        return "";
    }
    szFileName[ret] = '\0';
#endif
	return std::tstring(szFileName);
}

bool globalCreateDirectory(std::tstring Path)
{
    boost::filesystem::path file_path(Path);
    if(boost::filesystem::exists(file_path)) {
        return true;
    } else {
        return boost::filesystem::create_directories(file_path);//如果用create_directory则不能判断父目录是否存在
    }
}

bool globalTerminateProcess(std::tstring ProcessName)
{
    bool ReturnFlag = false;
#ifdef WIN32
	try
	{
		std::vector<HANDLE> ProcessVector;
        ProcessName = upperCase(ProcessName);
		PROCESSENTRY32 curPROCESSENTRY32;
		HANDLE curHandle;
		BOOL curFlag;
		std::tstring curExeStr;
		curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
		curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
		while (curFlag)
		{
            curExeStr = globalExtractFileName(static_cast<std::tstring>(curPROCESSENTRY32.szExeFile));
            curExeStr = upperCase(curExeStr);
			if (ProcessName == curExeStr)
			{
				HANDLE curhd = ::OpenProcess(PROCESS_TERMINATE, true, curPROCESSENTRY32.th32ProcessID);
				if (curhd) ProcessVector.push_back(curhd);
			}
			curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
		}
		::CloseHandle(curHandle);

		for (std::vector<HANDLE>::iterator it = ProcessVector.begin(); it != ProcessVector.end(); it++)
		{
            HANDLE curhd = static_cast<HANDLE>(*it);
			if (curhd)
			{
				try
				{
					::TerminateProcess(curhd, 0);
                    if (!ReturnFlag) ReturnFlag = true;
				}
				catch (...)
				{
					;
				}
			}
		}
	}
	catch (...)
	{
		;
	}
#else
    DIR *dir; //进程目录
    struct dirent * next;

    FILE *status;
    char buffer[1024];
    char name[1024];

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (!dir) //目录不存在结束
    {
        return false;
    }
    std::string cur_name;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(ProcessName.c_str(), ProcessName.length(), cur_name);
#else
    cur_name = ProcessName;
#endif

    //遍历 读取进程信息
    std::vector<int> pidVector;
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(static_cast<int>(*next->d_name))) continue;
        //判断是否能打开状态文件
        //sprintf(buffer,"/proc/%s/status",next->d_name);
        sprintf(buffer,"/proc/%s/cmdline",next->d_name);
        if (!(status = fopen(buffer,"r"))) continue;
        //读取状态文件
        if (fgets(buffer,1024,status) == nullptr)
        {
            fclose(status);
            continue;
        }
        fclose(status);
        //sscanf(buffer,"%*s %s",name); //读取PID对应的程序名，格式为Name:  程序名
        //if(strstr(name, cur_name.c_str()))
        sscanf(buffer,"%s",name);
        std::tstring activeName = globalExtractFileNameExcludeExt(name);
        //if(strstr(name, cur_name.c_str()))
        if (activeName == cur_name)
        {
            int cur_pid;
            globalStrToIntDef(next->d_name, cur_pid);
            if (cur_pid > 0) pidVector.push_back(cur_pid);
        }
    }

    for (std::vector<int>::iterator it = pidVector.begin(); it != pidVector.end(); it++)
    {
        kill(static_cast<__pid_t>(*it), SIGKILL);
        if (!ReturnFlag) ReturnFlag = true;
    }
    closedir(dir);
#endif // WIN32
    return ReturnFlag;
}

bool globalTerminateProcess(int ProcessId)
{
    bool ReturnFlag = false;
#ifdef WIN32
    try
    {
        PROCESSENTRY32 curPROCESSENTRY32;
        HANDLE curHandle;
        BOOL curFlag;
        std::tstring curExeStr;
        curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
        curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
        DWORD curProcessId = static_cast<DWORD>(ProcessId);
        while (curFlag)
        {
            if (curPROCESSENTRY32.th32ProcessID == curProcessId)
            {
                HANDLE curhd = ::OpenProcess(PROCESS_TERMINATE, true, curPROCESSENTRY32.th32ProcessID);
                if (curhd)
                {
                    try
                    {
                        ::TerminateProcess(curhd, 0);
                        if (!ReturnFlag) ReturnFlag = true;
                    }
                    catch (...)
                    {
                        ;
                    }
                }
                break;
            }
            curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
        }
        ::CloseHandle(curHandle);
    }
    catch (...)
    {
        ;
    }
#else
    DIR *dir; //进程目录
    struct dirent * next;

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (!dir) //目录不存在结束
    {
        return false;
    }

    //遍历 读取进程信息
    std::vector<int> pidVector;
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(static_cast<int>(*next->d_name))) continue;
        int cur_pid;
        globalStrToIntDef(next->d_name, cur_pid);
        if (cur_pid > 0 && cur_pid == ProcessId)
        {
            kill(static_cast<__pid_t>(cur_pid), SIGKILL);
            if (!ReturnFlag) ReturnFlag = true;
            break;
        }
    }
    closedir(dir);
#endif // WIN32
    return ReturnFlag;
}

bool globalTerminateProcess(std::vector<int> &vector, std::tstring &msg)
{
    bool ReturnFlag = false;
#ifdef WIN32
    try
    {
        PROCESSENTRY32 curPROCESSENTRY32;
        HANDLE curHandle;
        BOOL curFlag;
        std::tstring curExeStr;
        curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
        curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
        DWORD curProcessId;
        while (curFlag)
        {
            for (std::vector<int>::iterator it = vector.begin(); it != vector.end(); )
            {
                curProcessId = static_cast<DWORD>(*it);
                if (curPROCESSENTRY32.th32ProcessID == curProcessId)
                {
                    it = vector.erase(it);
                    HANDLE curhd = ::OpenProcess(PROCESS_TERMINATE, true, curPROCESSENTRY32.th32ProcessID);
                    if (curhd)
                    {
                        try
                        {
                            msg += convertToString(curProcessId) + _T(",");
                            ::TerminateProcess(curhd, 0);
                            if (!ReturnFlag) ReturnFlag = true;
                        }
                        catch (...)
                        {
                            ;
                        }
                    }
                    break;
                }
                else
                {
                    it++;
                }
            }
            if (vector.empty()) break;
            curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
        }
        ::CloseHandle(curHandle);
    }
    catch (...)
    {
        ;
    }
#else
    DIR *dir; //进程目录
    struct dirent * next;

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (!dir) //目录不存在结束
    {
        return false;
    }

    //遍历 读取进程信息
    std::vector<int> pidVector;
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(static_cast<int>(*next->d_name))) continue;
        int cur_pid;
        globalStrToIntDef(next->d_name, cur_pid);
        if (cur_pid <= 0) continue;
        for (std::vector<int>::iterator it = vector.begin(); it != vector.end(); )
        {
            DWORD ProcessId = static_cast<DWORD>(*it);
            if (static_cast<DWORD>(cur_pid) == ProcessId)
            {
                it = vector.erase(it);
                msg += convertToString(cur_pid) + _T(",");
                kill(static_cast<__pid_t>(cur_pid), SIGKILL);
                if (!ReturnFlag) ReturnFlag = true;
                break;
            }
            else
            {
                it++;
            }
        }
        if (vector.empty()) break;
    }
    closedir(dir);
#endif // WIN32
    return ReturnFlag;
}

bool globalIsProcessRun(std::tstring ProcessName)
{
	bool ReturnFlag = false;
#ifdef WIN32
	try
	{
        ProcessName = upperCase(ProcessName);
		std::tstring x64Name = ProcessName + _T(" *32");
		PROCESSENTRY32 curPROCESSENTRY32;
		HANDLE curHandle;
		BOOL curFlag;
		std::tstring curExeStr;
		curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
		curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
		while (curFlag)
		{
            curExeStr = globalExtractFileName(static_cast<std::tstring>(curPROCESSENTRY32.szExeFile));
            curExeStr = upperCase(curExeStr);
            //curPROCESSENTRY32.th32ProcessID
			if (ProcessName == curExeStr || x64Name == curExeStr)
			{
				ReturnFlag = true;
				break;
			}
			curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
		}
		::CloseHandle(curHandle);
	}
	catch (...)
	{
		;
	}
#else
    DIR *dir; //进程目录
    struct dirent * next;

    FILE *status;
    char buffer[1024];
    char name[1024];

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (!dir) //目录不存在结束
    {
        return ReturnFlag;
    }
    std::string cur_name;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(ProcessName.c_str(), ProcessName.length(), cur_name);
#else
    cur_name = ProcessName;
#endif

    //遍历 读取进程信息
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(static_cast<int>(*next->d_name))) continue;
        //判断是否能打开状态文件
        //sprintf(buffer,"/proc/%s/status",next->d_name);
        sprintf(buffer,"/proc/%s/cmdline",next->d_name);
        if (!(status = fopen(buffer,"r"))) continue;
        //读取状态文件
        if (fgets(buffer,1024,status) == nullptr)
        {
            fclose(status);
            continue;
        }
        fclose(status);
        //sscanf(buffer,"%*s %s",name); //读取PID对应的程序名，格式为Name:  程序名
        sscanf(buffer,"%s",name);
        std::tstring activeName = globalExtractFileNameExcludeExt(name);
        //if(strstr(name, cur_name.c_str()))
        if (activeName == cur_name)
        {
            ReturnFlag = true;
            break;
        }
    }
    closedir(dir);
#endif // WIN32
	return ReturnFlag;
}

bool globalIsProcessRun(int ProcessId)
{
    bool ReturnFlag = false;
#ifdef WIN32
    try
    {
        DWORD curProcessId = static_cast<DWORD>(ProcessId);
        PROCESSENTRY32 curPROCESSENTRY32;
        HANDLE curHandle;
        BOOL curFlag;
        std::tstring curExeStr;
        curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
        curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
        while (curFlag)
        {
            if (curPROCESSENTRY32.th32ProcessID == curProcessId)
            {
                ReturnFlag = true;
                break;
            }
            curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
        }
        ::CloseHandle(curHandle);
    }
    catch (...)
    {
        ;
    }
#else
    DIR *dir = nullptr; //进程目录
    struct dirent * next;

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (dir == nullptr) {//目录不存在结束
        return ReturnFlag;
    }

    //遍历 读取进程信息
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(*next->d_name)) continue;
        int cur_pid;
        globalStrToIntDef(next->d_name, cur_pid);
        if (cur_pid > 0 && cur_pid == ProcessId)
        {
            ReturnFlag = true;
            break;
        }
    }
    closedir(dir);
    dir = nullptr;
#endif // WIN32
    return ReturnFlag;
}

void globalGetProcessIdByName(std::tstring ProcessName, std::vector<int> &IdVector)
{
#ifdef WIN32
    try
    {
        std::vector<HANDLE> ProcessVector;
        ProcessName = upperCase(ProcessName);
        PROCESSENTRY32 curPROCESSENTRY32;
        HANDLE curHandle;
        BOOL curFlag;
        std::tstring curExeStr;
        curPROCESSENTRY32.dwSize = sizeof(PROCESSENTRY32);
        curHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        curFlag = ::Process32First(curHandle, &curPROCESSENTRY32);
        while (curFlag)
        {
            curExeStr = globalExtractFileName(static_cast<std::tstring>(curPROCESSENTRY32.szExeFile));
            curExeStr = upperCase(curExeStr);
            if (ProcessName == curExeStr)
            {
                IdVector.push_back(static_cast<int>(curPROCESSENTRY32.th32ProcessID));
            }
            curFlag = ::Process32Next(curHandle, &curPROCESSENTRY32);
        }
        ::CloseHandle(curHandle);
    }
    catch (...)
    {
        ;
    }
#else
    DIR *dir = nullptr; //进程目录
    struct dirent * next;

    FILE *status;
    char buffer[1024];
    char name[1024];

    dir = opendir("/proc"); ///proc中包括当前的进程信息,读取该目录
    if (!dir) {//目录不存在结束
        return;
    }
    std::string cur_name;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(ProcessName.c_str(), ProcessName.length(), cur_name);
#else
    cur_name = ProcessName;
#endif

    //遍历 读取进程信息
    std::vector<int> pidVector;
    while ((next = readdir(dir)) != nullptr)
    {
        //跳过"."和".."两个文件名
        if ((strcmp(next->d_name, "..") == 0) || (strcmp(next->d_name, ".") == 0)) continue;
        //如果文件名不是数字则跳过
        if (!isdigit(*next->d_name)) continue;
        //判断是否能打开状态文件
        //sprintf(buffer,"/proc/%s/status",next->d_name);
        sprintf(buffer,"/proc/%s/cmdline",next->d_name);
        if (!(status = fopen(buffer,"r"))) continue;
        //读取状态文件
        if (fgets(buffer,1024,status) == nullptr)
        {
            fclose(status);
            continue;
        }
        fclose(status);
        //sscanf(buffer,"%*s %s",name); //读取PID对应的程序名，格式为Name:  程序名
        //if(strstr(name, cur_name.c_str()))
        sscanf(buffer,"%s",name);
        std::tstring activeName = globalExtractFileNameExcludeExt(name);
        //if(strstr(name, cur_name.c_str()))
        if (activeName == cur_name)
        {
            int cur_pid;
            globalStrToIntDef(next->d_name, cur_pid);
            if (cur_pid > 0) IdVector.push_back(cur_pid);
        }
    }
    closedir(dir);
#endif // WIN32
}

bool globalDeleteFile(std::tstring strFileName)
{
#ifdef WIN32
    return DeleteFile((strFileName).c_str());
#else
    return (remove(strFileName.c_str()) != 0) ? false : true;
#endif
}

//递归删除指定文件夹
bool globalDeleteFolder(std::tstring strFilePath, std::tstring &ErrorMsg, bool RemoveDirectoryFlag)
{
    if (!globalDirectoryExists(strFilePath))
	{
        ErrorMsg = _T("文件夹") + strFilePath + _T("不存在");
		return false;
	}
    globalIncludePathDelimiter(strFilePath);
	bool SuccessFlag = true;
#ifdef WIN32
	std::tstring strFilter = _T("*.*");
	std::tstring strFile = strFilePath + strFilter;
	std::tstring strFileName;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strFile.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		ErrorMsg = _T("文件夹") + strFilePath + _T("打开失败");
		return false;
	}
	do
	{
		if (_tcscmp(wfd.cFileName, _T(".")) == 0 || _tcscmp(wfd.cFileName, _T("..")) == 0) continue;
		strFileName = strFilePath + wfd.cFileName;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
            if (!globalDeleteFolder(strFileName, ErrorMsg, true))
			{
				SuccessFlag = false;
				break;
			}			
		}
		else
		{
			if (!DeleteFile((strFileName).c_str()))
			{
                ErrorMsg = _T("删除文件 ") + strFileName + _T(" 失败，错误码：") + convertToString(static_cast<int>(GetLastError()));
				SuccessFlag = false;
			}
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);

	if (RemoveDirectoryFlag && SuccessFlag)
	{
		if (!RemoveDirectory(strFilePath.c_str()))
		{
            ErrorMsg = _T("删除文件夹 ") + strFilePath + _T("失败，错误码：") + convertToString(static_cast<int>(GetLastError()));
			SuccessFlag = false;
		}
	}
#else
    DIR *pDir = opendir(strFilePath.c_str());
	if (pDir == nullptr)
	{
		ErrorMsg = _T("文件夹") + strFilePath + _T("打开失败");
		return false;
	}
	struct dirent *dmsg;
	struct stat st;
    std::tstring strFileName, tempFileName;
    std::list<std::tstring> fileNameList;
	while ((dmsg = readdir(pDir)) != nullptr)
	{
		if (_tcscmp(dmsg->d_name, _T(".")) == 0 || _tcscmp(dmsg->d_name, _T("..")) == 0) continue;
		strFileName = strFilePath + dmsg->d_name;
        tempFileName = strFilePath + dmsg->d_name;
        memset(&st, 0, sizeof(struct stat));
        if (stat(strFileName.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            if (!globalDeleteFolder(strFileName, ErrorMsg, true)) {
				SuccessFlag = false;
				break;
			}
        } else {
            try {
                fileNameList.push_back(tempFileName);
            } catch(std::bad_alloc) {
                ;
            }

		}
	}

    for (std::list<std::tstring>::iterator it = fileNameList.begin(); it != fileNameList.end(); it++)
    {
        tempFileName = *it;
        if (remove(tempFileName.c_str()) != 0) {
            ErrorMsg = _T("删除文件 ") + strFileName + _T(" 失败，错误码：") + convertToString(errno);
            SuccessFlag = false;
        }
    }

    if (RemoveDirectoryFlag && SuccessFlag) {
        if (remove(strFilePath.c_str()) != 0) {
            ErrorMsg = _T("删除文件夹") + strFilePath + _T(" 失败，错误码：") + convertToString(errno);
			SuccessFlag = false;
		}
	}
	closedir(pDir);
#endif
	return SuccessFlag;
}

bool globalGetFolderFile(std::tstring strFilePath, int Type, std::vector<std::tstring> &FileVector, std::tstring &ErrorMsg, std::tstring Ext)
{
	FileVector.clear();
    if (!globalDirectoryExists(strFilePath))
	{
		ErrorMsg = _T("文件夹") + strFilePath + _T("不存在");
		return false;
	}
    globalIncludePathDelimiter(strFilePath);
    Ext = upperCase(Ext);
#ifdef WIN32
	std::tstring strFilter = _T("*.*");
	std::tstring strFile = strFilePath + strFilter;
	std::tstring strFileName;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(strFile.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		ErrorMsg = _T("文件夹") + strFilePath + _T("打开失败");
		return false;
	}
	do
	{
		if (_tcscmp(wfd.cFileName, _T(".")) == 0 || _tcscmp(wfd.cFileName, _T("..")) == 0) continue;
		strFileName = strFilePath + wfd.cFileName;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (Type != 1)
			{
				if (!Ext.empty()) continue;
				FileVector.push_back(strFileName);
			}
		}
		else
		{
			if (Type != 2)
			{
                if (!Ext.empty() && upperCase(globalExtractFileExt(strFileName)) != Ext) continue;
				FileVector.push_back(strFileName);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
#else
	DIR *pDir = opendir(strFilePath.c_str());
    if (pDir == nullptr) {
		ErrorMsg = _T("文件夹") + strFilePath + _T("打开失败");
		return false;
	}
	struct dirent *dmsg;
	struct stat st;
	std::tstring strFileName;
    while ((dmsg = readdir(pDir)) != nullptr) {
		if (_tcscmp(dmsg->d_name, _T(".")) == 0 || _tcscmp(dmsg->d_name, _T("..")) == 0) continue;
		strFileName = strFilePath + dmsg->d_name;
		memset(&st, 0, sizeof(struct stat));
        if (stat(strFileName.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            if (Type != 1) {
				if (!Ext.empty()) continue;
				FileVector.push_back(strFileName);
			}
        } else {
            if (Type != 2) {
                if (!Ext.empty() && upperCase(globalExtractFileExt(strFileName)) != Ext) continue;
				FileVector.push_back(strFileName);
			}
		}
	}
	closedir(pDir);
#endif // WIN32
    return true;
}

int my_isspace(int ch)
{
    //return static_cast<unsigned int>(ch - 9) < 5U || ch == ' ';
    int tempValue = ch - 9;
    bool tempBool = static_cast<unsigned int>(tempValue) < 5U || ch == ' ';
    return static_cast<int>(tempBool);
}

std::tstring &ltrim(std::tstring &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(my_isspace))));
	return s;
}

// trim from end 
std::tstring &rtrim(std::tstring &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(my_isspace))).base(), s.end());
	return s;
}

std::tstring &trim(std::tstring &s)
{
	return ltrim(rtrim(s));
}

void unicode_to_utf8(const wchar_t* in, size_t len, std::string& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-32", "UTF-8//IGNORE", save, w_len, (char*)(in), w_len);
    out = save;
    delete []save;
#endif
}

void utf8_to_unicode(const char* in, size_t len, std::wstring& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-8", "UTF-32//IGNORE", save, w_len, (char*)(in), len);
    out = (wchar_t*)save+1;
    delete[]save;
#endif
}

void ansi_to_unicode(const char* in, size_t len, std::wstring& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);

    iconv_convert("GBK", "UTF-32//IGNORE", save, w_len, (char*)(in), len);
    out = (wchar_t*)save+1;
    delete[]save;
#endif
}

void unicode_to_ansi(const wchar_t* in, size_t len, std::string& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-32", "GBK//IGNORE", save, w_len, (char*)(in), w_len - 1U);
    out = save;
    delete[]save;
#endif
}

void ansi_to_utf8(const char* in, size_t len, std::string& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("GBK", "UTF-8//IGNORE", save, w_len, (char*)(in), len);
    out = save;
    delete[]save;
#endif
}

void utf8_to_ansi(const char* in, size_t len, std::string& out)
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
    char *save = new char[w_len];
    memset(save, '\0', w_len);
    iconv_convert("UTF-8", "GBK//IGNORE", save, w_len, (char*)(in), len);
    out = save;
    delete[]save;
#endif
}

int globalStrToIntDef(const LPTSTR valuechar, int &value, int defaultvalue, int base)
{
	if (base < 2 || base > 16) base = 10;
	value = defaultvalue;
#if defined(UNICODE) || defined(_UNICODE)
	wchar_t *endptr;
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
	char *endptr;
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

int globalStrToInt(const LPTSTR valuechar, int defaultvalue, int base)
{
    if (base < 2 || base > 16) base = 10;
#if defined(UNICODE) || defined(_UNICODE)
    wchar_t *endptr;
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
    char *endptr;
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

void globalCreateGUID(std::tstring &GuidStr)
{
/*#ifdef WIN32
	::CoInitialize(NULL);
	GUID guid;
	if (::CoCreateGuid(&guid) == S_OK)
	{
		_sntprintf_s(GuidStr, length, _TRUNCATE, _T("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"),
			guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
			guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	::CoUninitialize();
#else
	GUID guid;
    uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#endif // WIN32*/
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();

	std::string result;
    result.reserve(36U);

    std::size_t i = 0U;
	
	for (boost::uuids::uuid::const_iterator it_data = a_uuid.begin(); it_data != a_uuid.end(); ++it_data, ++i) {
        unsigned char tempchar = ((*it_data) >> 4) & 0x0F;
        const size_t hi = static_cast<size_t>(tempchar);
		result += boost::uuids::detail::to_char(hi);
        unsigned char tempLo = (*it_data) & 0x0FU;
        const size_t lo = static_cast<size_t>(tempLo);
		result += boost::uuids::detail::to_char(lo);
	}
#if defined(UNICODE) || defined(_UNICODE)
    ansi_to_unicode(result.c_str(), result.length(), GuidStr);
#else
	GuidStr = result;
#endif
}

void globalCreateGUID_Line(std::tstring &GuidStr)
{
	/*::CoInitialize(NULL);
	GUID guid;
	if (::CoCreateGuid(&guid) == S_OK)
	{
		_sntprintf_s(GuidStr, length, _TRUNCATE, _T("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
			guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
			guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	::CoUninitialize();*/
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
	GuidStr = boost::uuids::to_string(a_uuid);
}

bool dealCheckGuid_One(std::tstring &str, std::tstring::size_type begin, std::tstring::size_type end)
{
    char var;
    for (std::tstring::size_type i = begin; i <= end; i++)
    {
        var = str.at(i);
        if ((var < '0' || var > '9') && (var < 'a' || var > 'f')) return false;
    }
    return true;
}

bool globalCheckGUID_One(const std::tstring &guidStr, std::tstring &err)
{
    std::vector<std::tstring> vector;
    std::tstring checkStr;
    globalSpliteString(guidStr, vector, _T("-"));
    if (vector.size() != 5U) {
        err = _T("长度不正确");
        return false;
    }
    checkStr = *(vector.begin());
    if (checkStr.substr(0, 1) != _T("{")) {
        err = _T("必须以{开头");
        return false;
    }

    if (checkStr.length() != 9U) {
        err = _T("第一部分长度不正确");
        return false;
    }
    if (!dealCheckGuid_One(checkStr, 1U, (checkStr.length() - 1))) {
        err = _T("第一部分包含0-9和a-f之外字符");
        return false;
    }

    checkStr = *(vector.begin() + 1);
    if (checkStr.length() != 4U) {
        err = _T("第二部分长度不正确");
        return false;
    }
    if (!dealCheckGuid_One(checkStr, 0U, (checkStr.length() - 1))) {
        err = _T("第二部分包含0-9和a-f之外字符");
        return false;
    }

    checkStr = *(vector.begin() + 2);
    if (checkStr.length() != 4U) {
        err = _T("第三部分长度不正确");
        return false;
    }
    if (!dealCheckGuid_One(checkStr, 0U, (checkStr.length() - 1))) {
        err = _T("第三部分包含0-9和a-f之外字符");
        return false;
    }

    checkStr = *(vector.begin() + 3);
    if (checkStr.length() != 4U) {
        err = _T("第四部分长度不正确");
        return false;
    }
    if (!dealCheckGuid_One(checkStr, 0U, (checkStr.length() - 1))) {
        err = _T("第四部分包含0-9和a-f之外字符");
        return false;
    }

    checkStr = *(vector.begin() + 4);
    if (checkStr.substr(checkStr.length() - 1, 1) != _T("}")) {
        err = _T("必须以}结束");
        return false;
    }
    if (checkStr.length() != 13U) {
        err = _T("第五部分长度不正确");
        return false;
    }
    if (!dealCheckGuid_One(checkStr, 0U, (checkStr.length() - 2))) {
        err = _T("第五部分包含0-9和a-f之外字符");
        return false;
    }
    return true;
}

bool globalIsIPStringValid(std::tstring IPString)
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

bool globalIsMaskStringValid(std::tstring MaskString)
{
    std::vector<std::tstring> vector;
    globalSpliteString(MaskString, vector, _T("."));
    if (vector.size() != 4U) return false;
    int Value, Temp;
    bool ZeroFlag = false;
    for (std::vector<std::tstring>::iterator it = vector.begin(); it != vector.end(); it++)
    {
        globalStrToIntDef(const_cast<LPTSTR>((*it).c_str()), Value);
        if (Value < 0 || Value > 255) return false;
        for (int i = 7; i >= 0; i--)
        {
            Temp = (Value >> i) & 0x01;
            if (Temp == 0) {
                if (!ZeroFlag) ZeroFlag = true;
            }
            else {
                if (ZeroFlag) return false;
            }
        }
    }
    return true;
}

bool globalIsMultiIpValid(std::tstring IPString, bool useFlag)
{
	std::string strIpAddr;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(IPString.c_str(), IPString.length(), strIpAddr);
#else
	strIpAddr = IPString;
#endif
    std::string::size_type nbytes = strIpAddr.length();
	int num = 0;
    for (std::string::size_type i = 0U; i < nbytes; i++)
	{
		if (strIpAddr.at(i) == '.') num++;
	}
	if (num != 3)
	{
		return false;
	}

	struct in_addr addr;
    //if (inet_pton(AF_INET, strIpAddr.c_str(), (void *)&addr) == 1)
    if (inet_pton(AF_INET, strIpAddr.c_str(), reinterpret_cast<void*>(&addr)) == 1)
	{
        INT64 tempValue = globalGetIpByString(IPString);
        if (useFlag) {
            if (tempValue < 0xE0000200 || tempValue > 0xEEFFFFFF) return false;
        } else {
            if (tempValue < 0xE0000000 || tempValue > 0xEFFFFFFF) return false;
        }

		return true;
	}
	else
	{
		return false;
	}
}

DWORD globalIP2DWORD(std::tstring IpAddr)
{
#ifdef WIN32
    struct in_addr addr;
    if (inet_pton(AF_INET, IpAddr.c_str(), reinterpret_cast<void*>(&addr)) == 1)
    {
        return addr.S_un.S_addr;
    }
    return 0;
#else
    //struct sockaddr_in addr;
    struct in_addr addr;
    if (inet_pton(AF_INET, IpAddr.c_str(), reinterpret_cast<void*>(&addr)) == 1)
    {
        //return addr.sin_addr.s_addr;
        return addr.s_addr;
    }
    return 0U;
#endif
}

INT64 globalGetIpByString(std::tstring IpAddr)
{
	int IpAddrInt[4];
    UINT IpAddrUInt[4];
	std::tstring IpAddrTemp = IpAddr;
	for (int i = 0; i < 4; i++)
	{
        std::tstring::size_type TempIpAddrPos = IpAddrTemp.find(_T("."), 0U);
        std::tstring TempIpStr;
        if (TempIpAddrPos > 0U)
            TempIpStr = IpAddrTemp.substr(0U, TempIpAddrPos);
        else
            TempIpStr = IpAddrTemp;
        //globalStrToIntDef((LPTSTR)(TempIpStr.c_str()), IpAddrInt[i]);
        globalStrToIntDef(const_cast<LPTSTR>(TempIpStr.c_str()), IpAddrInt[i]);
        if (IpAddrInt[i] < 0) return 0;
        IpAddrInt[i] = IpAddrInt[i] & 0x000000ff;
        IpAddrUInt[i] = static_cast<unsigned int>(IpAddrInt[i]);
        IpAddrTemp = IpAddrTemp.substr(TempIpAddrPos + 1U, IpAddrTemp.length() - TempIpAddrPos - 1U);
	}
    UINT tempValue = ((IpAddrUInt[0] << 24) & 0xff000000U) | ((IpAddrUInt[1] << 16) & 0x00ff0000U) | ((IpAddrUInt[2] << 8) & 0x0000ff00U) | (IpAddrUInt[3]);
    INT64 IpAddr_Binary = static_cast<INT64>(tempValue);
    //INT64 IpAddr_Binary = ((static_cast<unsigned int>(IpAddrInt[0] & 0x000000ff) << 24) & 0xff000000) | ((static_cast<unsigned int>(IpAddrInt[1] & 0x000000ff) << 16) & 0x00ff0000)
    //        | ((static_cast<unsigned int>(IpAddrInt[2] & 0x000000ff) << 8) & 0x0000ff00) | static_cast<unsigned int>(IpAddrInt[3] & 0x000000ff);
	return IpAddr_Binary;
}

void globalGetHostIpAddr(std::vector<std::tstring> &IpList)
{
	IpList.clear();
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	char cHostName[256];
	if (gethostname(cHostName, sizeof(cHostName)) < 0)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return;
	}
    struct addrinfo hints;
    addrinfo *res, *cur;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
    int ret = getaddrinfo(cHostName, nullptr, &hints, &res);
	if (ret != 0)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return;
	}
	struct sockaddr_in *addr;
	char cIpAddr[128];
    int maxCount = 32;
    for (cur = res; cur != nullptr; cur = cur->ai_next)
	{
        maxCount--;
        if (maxCount <= 0) break;
        //if (cur->ai_canonname)
        memset(cIpAddr, 0, sizeof(cIpAddr));
        addr = reinterpret_cast<struct sockaddr_in*>(cur->ai_addr);//(struct sockaddr_in *)cur->ai_addr;
        const char* ret = inet_ntop(AF_INET, &addr->sin_addr.s_addr, cIpAddr, static_cast<UINT>(sizeof(cIpAddr)));
        if (ret == nullptr)	continue;
        std::tstring ipStr;
#if defined(UNICODE) || defined(_UNICODE)
        std::string tempstr = ipChar;
        ANSI_to_Unicode(tempstr.c_str(), tempstr.length(), ipStr);
#else
        ipStr = cIpAddr;
#endif
        if (globalIsIPStringValid(ipStr))
        {
            bool findflag = false;
            for (std::vector<std::tstring>::iterator it = IpList.begin(); it != IpList.end(); it++)
            {
                if (*it == ipStr)
                {
                    findflag = true;
                    break;
                }
            }
            if (!findflag) IpList.push_back(ipStr);
        }
	}
	freeaddrinfo(res);
#ifdef WIN32
	WSACleanup();
#endif
}

bool globalFileExists(std::tstring FileName)
{
#ifdef WIN32
    HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//DWORD dwError = ::GetLastError();
		//if (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND || dwError == ERROR_INVALID_NAME || dwError == ERROR_BAD_NETPATH)
		//{
		//	
		//}
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return true;
#else
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName;
    unicode_to_ansi(FileName.c_str(), FileName.length(), strName);
	return (access(strName.c_str(), F_OK) == 0) ? true : false;
#else
	return (access(FileName.c_str(), F_OK) == 0) ? true : false;
#endif
#endif // WIN32
}

bool globalDirectoryExists(std::tstring Directory)
{
#ifdef WIN32
	DWORD dwAttr = ::GetFileAttributes(Directory.c_str());
	if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
	/*WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(Directory.c_str(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE && wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return true;
	else return false;*/
#else
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName;
    unicode_to_ansi(Directory.c_str(), Directory.length(), strName);
	return (access(strName.c_str(), F_OK) == 0) ? true : false;
#else
	return (access(Directory.c_str(), F_OK) == 0) ? true : false;
#endif
#endif // WIN32
	/*if (0 == _tcsncicmp(Directory.c_str(), _T("ftp://"), _tcslen(_T("ftp://"))))
	{
		std::tstring fn = Directory;
		HANDLE hInternet = NULL, hFtp = NULL;
		try
		{
			static const int _MAX_NAME = 32;
			TCHAR user[_MAX_NAME], pwd[_MAX_NAME], host[_MAX_NAME], dir[_MAX_DIR], name[_MAX_FNAME], ext[_MAX_EXT];
			unsigned short port = 21;
			int flag = _tsplitftp_s(fn.c_str(), user, _countof(user), pwd, _countof(pwd), host, _countof(host), port, dir, _countof(dir), name, _countof(name), ext, _countof(ext));
			if (flag)
				throw (int)__LINE__;

			//ftp_type ft = ft_unknown;
			//getFtpType(host, port, ft);

			hInternet = ::InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			if (NULL == hInternet)
				throw (int)__LINE__;

			hFtp = ::InternetConnect(hInternet, host, port, user, pwd, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
			if (NULL == hFtp)
				throw (int)__LINE__;

			::FtpCommand(hFtp, false, FTP_TRANSFER_TYPE_ASCII, _T("OPTS UTF8 OFF"), NULL, NULL);

			TCHAR pathname[_MAX_PATH], *pb = dir, *pe = 0;
			while (*pb)
			{
				pe = _tcschr(pb, '/');
				if (pe == pb)
				{
					pb = pe + 1;
					continue;
				}
				_tcsncpy_s(pathname, _countof(pathname), pb, pe ? pe - pb : _tcslen(pb));
				if (!::FtpSetCurrentDirectory(hFtp, pathname))
					throw (int)__LINE__;

				if (pe)
					pb = pe + 1;
				else
					break;
			}

			throw 0;
		}
		catch (int& e)
		{
			errno = e;

			if (hFtp)
				::InternetCloseHandle(hFtp);
			if (hInternet)
				::InternetCloseHandle(hInternet);

			return e ? false : true;
		}
	}
	else
	{
		DWORD dwAttr = ::GetFileAttributes(Directory.c_str());
		if (dwAttr == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}
		return false;
	}*/
}

INT64 globalGetFileSize(std::tstring FileName)
{
#ifdef WIN32
    HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) return 0;
	LARGE_INTEGER largeInt;
	largeInt.QuadPart = 0;
    if (GetFileSizeEx(hFile, &largeInt) == 0 || largeInt.QuadPart <= 0) {
		CloseHandle(hFile);
		return 0;
    } else{
		CloseHandle(hFile);
		return largeInt.QuadPart;
	}
#else
	struct stat statbuf;
    std::string fname;
#if defined(UNICODE) || defined(_UNICODE)
    unicode_to_ansi(FileName.c_str(), FileName.length(), fname);
#else
    fname = FileName;
#endif
    if (stat(fname.c_str(), &statbuf) == 0)
		return statbuf.st_size;
    return 0;
#endif // WIN32	
}

std::tstring globalFileSizeShow(INT64 &FileSize)
{
	TCHAR strSize[16];
	::memset(strSize, 0, sizeof(TCHAR) * 16);
	if (FileSize < 0) return UNDEFINESTRING;
    else if (FileSize < 1024) _sntprintf_s(strSize, _countof(strSize), _TRUNCATE, _T("%dB"), static_cast<int>(FileSize));
    else if (FileSize < 1048576) _sntprintf_s(strSize, _countof(strSize), _TRUNCATE, _T("%.2fKB"), (static_cast<double>(FileSize)) / 1024.0 - 0.005);
    else if (FileSize < 1073741824) _sntprintf_s(strSize, _countof(strSize), _TRUNCATE, _T("%.2fMB"), (static_cast<double>(FileSize)) / 1024.0 / 1024.0 - 0.005);
    else if (FileSize < 1099511627776) _sntprintf_s(strSize, _countof(strSize), _TRUNCATE, _T("%.2fGB"), (static_cast<double>(FileSize)) / 1024.0 / 1024.0 / 1024.0 - 0.005);
    else _sntprintf_s(strSize, _countof(strSize), _TRUNCATE, _T("%.2fTB"), static_cast<double>(FileSize) / 1024.0 / 1024.0 / 1024.0 / 1024.0 - 0.005);
	std::tstring str = strSize;
	return str;
}

bool globalDecToHex(unsigned int idec, std::tstring &str)
{
	TCHAR tempchar[4];
	memset(tempchar, 0, sizeof(TCHAR) * 4);
#ifdef WIN32
    _sntprintf_s(tempchar, 4, _TRUNCATE, _T("%02X"), idec);
#else
    _snprintf(tempchar, 4U, _T("%02X"), idec);
#endif // WIN32	
	str = tempchar;
	return true;
}

bool globalHexToDec(LPCTSTR shex, unsigned int& idec)
{
	int i, mid;
    size_t tempLen = static_cast<size_t>(lstrlen(shex));
    int len = static_cast<int>(tempLen);
	if (len > 8) return false;
    mid = 0;
    idec = 0U;
	for (i = 0; i < len; i++)
	{
		if (shex[i] >= '0'&&shex[i] <= '9')   mid = shex[i] - '0';
		else   if (shex[i] >= 'a'&&shex[i] <= 'f')   mid = shex[i] - 'a' + 10;
		else   if (shex[i] >= 'A'&&shex[i] <= 'F')   mid = shex[i] - 'A' + 10;
		else   return   FALSE;
		mid <<= ((len - i - 1) << 2);
        idec |= (static_cast<unsigned int>(mid));
	}
	return true;
}

void globalBufToShowString(LPBYTE buf, UINT size, std::tstring &showStr)
{
    if (buf == nullptr) return;
    std::tstring tempstr;
    short tempvalue;
    for (UINT i = 0U; i < size; i++)
    {
        tempvalue = buf[i] & 0xff;
        globalDecToHex(static_cast<unsigned int>(tempvalue), tempstr);
        if (i == 0U) showStr = tempstr;
        else showStr = showStr + _T(" ") + tempstr;
    }
}

int globalRound(double value)
{
    if (value >= EPISON) {
        double cur = fabs(value) + 0.5;
        return static_cast<int>(cur);
    }
    else if (value <= -EPISON) {
        double cur = fabs(value) + 0.5;
        return -1*static_cast<int>(cur);
    }
    else {
        return 0;
    }
}

double globalRound(double value, int precision)
{
    long temp = ((static_cast<long>(value * pow(10, (precision + 1)))) + 5) / 10;
    return static_cast<double>(temp)/pow(10, (precision));
}

bool atoi64_t(char *arrTmp, INT64 &value)
{
    value = 0U;
    int len = 0;
    int i = 0;
    int j = 0;
    INT64 nTmpRes = 0U;
    INT64 ntmp10 = 1U;
    if (arrTmp == nullptr)
    {
        return false;
    }
    len = static_cast<int>(strlen(arrTmp));
    char tempChar;
    for (i = len-1; i >= 0; i--)
    {
        ntmp10 =1U;
        for (j = 1; j < (len -i); j++)
        {
            ntmp10 = ntmp10*10U;
        }
        tempChar = arrTmp[i]-48;
        nTmpRes = nTmpRes + static_cast<INT64>(tempChar) * ntmp10;
    }
    value = nTmpRes;
    return true;
}

bool atoiu64_t(char *arrTmp, UINT64 &value)
{
    value = 0U;
    int len = 0;
    int i = 0;
    int j = 0;
    UINT64 nTmpRes = 0U;
    UINT64 ntmp10 = 1U;
    if (arrTmp == nullptr)
    {
        return false;
    }
    len = static_cast<int>(strlen(arrTmp));

    char tempChar;
    for (i = len-1; i >= 0; i--)
    {
        ntmp10 =1U;
        for (j = 1; j < (len -i); j++)
        {
            ntmp10 = ntmp10*10U;
        }
        tempChar = arrTmp[i]-48;
        nTmpRes = nTmpRes + static_cast<UINT64>(tempChar) * ntmp10;
    }
    value = nTmpRes;
    return true;
}

bool itoa64_t(INT64 n, char *p, int len)
{
    char arrTmp[128] ;
    int i = 0;
    int j = 0;
    INT64 tmp;
    INT64 numTmp = 0;
    if (p == nullptr)
    {
        return false;
    }
    memset(p, 0, static_cast<size_t>(len));

    for (i = 0; ; i++)
    {
        numTmp = n % 10;
        tmp = n / 10;
        if (numTmp == 0 && tmp == 0)
        {
            break;
        }
        arrTmp[i] = static_cast<char>(numTmp + 48);
        n= tmp;
    }
    arrTmp[i] =0;
    if (len < static_cast<int>(strlen(arrTmp)))
    {
        return false;
    }
    else
    {
        int tempStrLen = static_cast<int>(strlen(arrTmp));
        for (i = tempStrLen - 1, j = 0; i >= 0; i--,j++)
        {
            p[j] = arrTmp[i];
        }
    }
    return true;
}

bool itoa64_t(INT64 n, std::tstring &str)
{
    char p[128];
    if (itoa64_t(n, p, 128))
    {
        str = p;
        return true;
    }
    else
    {
        return false;
    }
}

bool itoau64_t(UINT64 n, char *p, int len)
{
    char arrTmp[128] ;
    int i = 0;
    int j = 0;
    UINT64 tmp, tmp1;
    UINT64 numTmp = 0U;
    if (p == nullptr)
    {
        return false;
    }
    memset(p, 0, static_cast<size_t>(len));

    for (i = 0; ; i++)
    {
        numTmp = n % 10U;
        tmp = n / 10U;
        if (numTmp == 0U && tmp == 0U)
        {
            break;
        }
        tmp1 = numTmp + 48U;
        arrTmp[i] = static_cast<char>(tmp1);
        n= tmp;
    }
    arrTmp[i] =0;
    if (len < static_cast<int>(strlen(arrTmp))) {
        return false;
    } else {
        size_t tempLen = strlen(arrTmp)-1U;
        for (i = static_cast<int>(tempLen), j = 0; i >= 0; i--,j++)
        {
            p[j] = arrTmp[i];
        }
    }
    return true;
}

bool itoau64_t(UINT64 n, std::tstring &str)
{
    char p[128];
    if (itoau64_t(n, p, 128))
    {
        str = p;
        return true;
    }
    else
    {
        return false;
    }
}

UINT64 atoui64_t(const char *str)
{
    if (str == nullptr) return 0U;
    int a[]={10,11,12,13,14,15};
    UINT64 ret=0U;
    int tempValue;
    for(int i=0;str[i]!='\0';i++)
    {
        ret *= 16U;
        if(str[i] >= 0 && str[i] <= '9') {
            tempValue = str[i] - '0';
            ret += static_cast<UINT64>(tempValue);
        } else {
            tempValue = a[str[i]-'a'];
            ret += static_cast<UINT64>(tempValue);
        }
    }
    return ret;
}

std::tstring globalSerializeTimePoint( const std::chrono::system_clock::time_point &time, const std::string &format, bool bIncludeMS)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    auto tNow = std::chrono::system_clock::now();
    //std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    //std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    auto tSeconds = std::chrono::duration_cast<std::chrono::seconds>(tNow.time_since_epoch());
    std::tm tm;
#ifdef WIN32
    //gmtime_s(&tm, &tt);
    localtime_s(&tm, &tt);
#else
    //tm = *std::gmtime(&tt);
    tm = *std::localtime(&tt);
#endif // WIN32
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );

    if (bIncludeMS) {
        auto tMilli = std::chrono::duration_cast<std::chrono::milliseconds>(tNow.time_since_epoch());
        auto ms = tMilli - tSeconds;
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    }
    return ss.str();
}

std::tm globalSystemClock2LocalTime(std::chrono::system_clock::time_point time)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm;
#ifdef WIN32
    localtime_s(&tm, &tt);
#else
    tm = *std::localtime(&tt);
#endif // WIN32
    return tm;
}

void globalResolveAddrStr(const std::tstring &str, std::tstring &protocol, std::tstring &addr, int &port, std::tstring &path, std::tstring &param, std::tstring &user, std::tstring &pwd)
{
    protocol = _T("");
    addr = _T("");
    path = _T("");
    param = _T("");
    user = _T("");
    pwd = _T("");
    port = 0;
    if (str.empty()) return;
    std::tstring checkStr = str;
    std::tstring::size_type pos;
    pos = str.find(_T("://"));

    if (pos != std::tstring::npos) {
        protocol = checkStr.substr(0, pos);
        checkStr = checkStr.substr(pos + 3, checkStr.length() - pos - 3);
    }

    pos = checkStr.rfind(_T("@"));//用户名和密码的解析
    std::tstring authStr;
    if (pos != std::tstring::npos) {
        authStr = checkStr.substr(0, pos);
        checkStr = checkStr.substr(pos + 1, checkStr.length() - pos - 1);
        pos = authStr.find(_T(":"));
        if (pos != std::tstring::npos) {
            user = authStr.substr(0, pos);
            pwd = authStr.substr(pos + 1, authStr.length() - pos - 1);
        } else {
            user = authStr;
        }

    }
    pos = checkStr.find(_T("?"));
    if (pos != std::tstring::npos) {//问号之后的是路径参数
        param = checkStr.substr(pos + 1, checkStr.length() - pos - 1);
        checkStr = checkStr.substr(0, pos);
    }
    pos = checkStr.find(_T("/"));
    if (pos != std::tstring::npos) {//第一个斜杆后面都是路径
        path = checkStr.substr(pos + 1, checkStr.length() - pos - 1);
        checkStr = checkStr.substr(0, pos);
    }
    pos = checkStr.find(_T(":"));
    if (pos != std::tstring::npos) {//冒号后面为端口号
        std::tstring portstr = checkStr.substr(pos + 1, checkStr.length() - pos - 1);
        globalStrToIntDef(const_cast<LPTSTR>(portstr.c_str()), port);
        checkStr = checkStr.substr(0, pos);
    } else {
        port = 80;//如果没有冒号分隔，表示默认80端口
    }
    addr = checkStr;
}
//--db=mysql;uhdplayout;uhdplayout;192.168.56.128:1521/orcl
void globalResolveDbStr(const std::tstring &str, int &type, std::tstring &addr, std::tstring &dbname, int &port, std::tstring &user, std::tstring &pwd)
{
    addr = _T("");
    user = _T("");
    pwd = _T("");
    dbname = _T("");
    port = 3306;
    type = 1;//0=orcle, 1=mysql
    if (str.empty()) return;
    std::string strType,strPort;
    std::tstring checkStr = str;
    std::tstring::size_type pos;
    pos = str.find(_T(";"));

    //type
    if (pos != std::tstring::npos) {
        strType = checkStr.substr(0U, pos);
        if(0U == strType.find("mysql"))
            type = 1;
        else if(0U == strType.find("oracle"))
            type = 0;
        else
            type = -1;
        checkStr = checkStr.substr(pos + 1U, checkStr.length() - pos - 1U);
    }
    //user
    pos = checkStr.find(_T(";"));
    std::tstring authStr;
    if (pos != std::tstring::npos) {
        user = checkStr.substr(0U, pos);
        checkStr = checkStr.substr(pos + 1U, checkStr.length() - pos - 1U);
    }
    //pwd
    pos = checkStr.find(_T(";"));
    if (pos != std::tstring::npos) {
        pwd = checkStr.substr(0U, pos);
        checkStr = checkStr.substr(pos + 1U, checkStr.length() - pos - 1U);
    }
    //dbname
    pos = checkStr.find(_T(":"));
    if (pos != std::tstring::npos) {
        addr = checkStr.substr(0U, pos);
        checkStr = checkStr.substr(pos + 1U, checkStr.length() - pos - 1U);
    }
    pos = checkStr.find(_T("/"));
    if (pos != std::tstring::npos) {
        strPort = checkStr.substr(0U, pos);
        globalStrToIntDef(const_cast<LPTSTR>(strPort.c_str()), port);
        checkStr = checkStr.substr(pos + 1U, checkStr.length() - pos - 1U);
    }
    dbname = checkStr;
}

void GlobalPrintTimecode(TCHAR *out, int nBufferSize, DWORD Value, double frameRate)
{
    //static const double DOUBLE_FPS[] = { 0, 24000.0 / 1001.0, 24.0, 25.0, 30000.0 / 1001.0, 30.0, 50.0, 60000.0 / 1001.0, 60.0 };
    if (frameRate < 0.0005) frameRate = 25.0;
    //if (frameRateIndex > 0 && frameRateIndex  < 9)
    //{
    //	framerate = DOUBLE_FPS[frameRateIndex];
    //}
    int millValue = static_cast<int>(static_cast<double>(Value) * 1000.0 / frameRate);
    int hourValue = millValue / 3600000;
    millValue = millValue % 3600000;
    int minuteValue = millValue / 60000;
    millValue = millValue % 60000;
    int secondValue = millValue / 1000;
    millValue = millValue % 1000;
    int frameValue = static_cast<int>(static_cast<double>(millValue) * frameRate / 1000.0);
    _sntprintf_s(out, nBufferSize, _TRUNCATE, _T("%02d:%02d:%02d:%02d"), hourValue, minuteValue, secondValue, frameValue);
}

void globalPrintTimecode(std::tstring &Str, DWORD Value, double frameRate)
{
    if (frameRate < 0.0005) frameRate = 25.0;
    int millValue = static_cast<int>(static_cast<double>(Value) * 1000.0 / frameRate);
    int hourValue = millValue / 3600000;
    millValue = millValue % 3600000;
    int minuteValue = millValue / 60000;
    millValue = millValue % 60000;
    int secondValue = millValue / 1000;
    millValue = millValue % 1000;
    int frameValue = static_cast<int>(static_cast<double>(millValue) * frameRate / 1000.0);
    TCHAR tempChar[12];
    memset(tempChar, 0, 12);
    _sntprintf_s(tempChar, 12, _TRUNCATE, _T("%02d:%02d:%02d:%02d"), hourValue, minuteValue, secondValue, frameValue);
    Str = tempChar;
}

double GlobalGetFrameRateByIndex(int index)
{
    static const double DOUBLE_FPS[] = { 0, 24000.0 / 1001.0, 24.0, 25.0, 30000.0 / 1001.0, 30.0, 50.0, 60000.0 / 1001.0, 60.0 };
    double frameRate = 25.0;
    if (index > 0 && index  < 9)
    {
        frameRate = DOUBLE_FPS[index];
    }
    return frameRate;
}

INT64 getSMPTE2059TAI_Time()
{
#ifdef WIN32
    FILETIME  ft;
    memset(&ft, 0, sizeof(FILETIME));
    LARGE_INTEGER uLargeTime;
    memset(&uLargeTime, 0, sizeof(LARGE_INTEGER));
    GetSystemTimeAsFileTime(&ft);
    ////2017.1.1 later the currentUtcOffset =+37seconds;TAI=UTC+currentUtcOffset;
    ///FileTime 1601.1.1 100ns unit
    ////UTC     1970.1.1  ns unit
    ////TAI=FileTime-116444736000000000
    uLargeTime.HighPart = static_cast<long>(ft.dwHighDateTime);
    uLargeTime.LowPart = ft.dwLowDateTime;
    uLargeTime.QuadPart -= TICKS_1601_TO_1970;////UTC时间 100ns单位
    //uLargeTime.QuadPart += CUR_LEAPSECONDS_FROM_1970*TICKSPERSEC;///UTC时间 100ns单位 --暂时没有明确是否要补充闰秒，先注释，等明确后再说
    uLargeTime.QuadPart *= 100LL;///TAIC时间 ns单位
    return uLargeTime.QuadPart;
#else
    struct timespec ts = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    INT64 value = static_cast<INT64>(ts.tv_sec) * TICKSPERNSEC + static_cast<INT64>(ts.tv_nsec);
    value += CUR_LEAPSECONDS_FROM_1970*TICKSPERSEC;//暂时没有明确是否要补充闰秒，先注释，等明确后再说
    return value;
#endif
}

void getSMPTE2059TAI_Time(TCHAR *str, int len, INT64 value)
{
    INT64 curValue = (value < 0) ? getSMPTE2059TAI_Time() : value;
    INT64 curSec = curValue / TICKSPERNSEC;
    INT64 curNan = curValue - curSec * TICKSPERNSEC;
#ifdef WIN32
    if (str) _sntprintf_s(str, len, _TRUNCATE, _T("%I64d:%I64d"), curSec, curNan);
#else
    if (str) _sntprintf_s(str, len, _TRUNCATE, _T("%lld:%lld"), curSec, curNan);
#endif
}

void getSMPTE2059TAI_Time(std::tstring &str, INT64 value)
{
    INT64 curValue = (value < 0) ? getSMPTE2059TAI_Time() : value;
    INT64 curSec = curValue / TICKSPERNSEC;
    INT64 curNan = curValue - curSec * TICKSPERNSEC;
    TCHAR tempchar[MAX_PATH];
    ::memset(tempchar, 0, sizeof(TCHAR)* MAX_PATH);
#ifdef WIN32
    _sntprintf_s(tempchar, MAX_PATH, _TRUNCATE, _T("%I64d:%I64d"), curSec, curNan);
#else
    _sntprintf_s(tempchar, MAX_PATH, _TRUNCATE, _T("%lld:%lld"), curSec, curNan);
#endif
    str = tempchar;
}

INT64 ConvertSMPTE2059TAI_Time(const std::tstring str)
{
    size_t pos = str.find(_T(":"));
    if (pos == std::tstring::npos) return 0;
    std::tstring secStr, nanStr;
    secStr = str.substr(0, pos);
    nanStr = str.substr(pos + 1, str.length() - pos - 1);
    INT64 curSec = _atoi64(secStr.c_str());
    INT64 curNan = _atoi64(nanStr.c_str());
    return ((curSec * TICKSPERNSEC) + curNan);
}
