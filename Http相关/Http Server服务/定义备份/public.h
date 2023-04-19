#pragma once
#include <iomanip>
#include "common.h"
#include "jsoncpp/json/json.h"

#ifndef WIN32
#define UINT uint32_t
#define UINT64 unsigned long long
#define INT64 long long
#endif

extern TCHAR g_ComputerName[MAX_PATH];
extern std::tstring    g_ComputerNameStr;
extern std::tstring    g_ComputerNameUpper;

#define SECSPERDAY 86400LL /* one day seconds */
#define SECS_1601_TO_1970 ((369LL * 365LL + 89LL) * SECSPERDAY)/* 1601 to 1970 is 369 years plus 89 leap days */
#define CUR_LEAPSECONDS_FROM_1970  37LL  //from 1970.1.1 to 2017-9-29 LeapSeconds
#define TICKSPERSEC 10000000LL //1s内的100纳秒间隔
#define TICKSPERNSEC 1000000000LL //1s内的100纳秒间隔
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)

#define EPISON 1e-7

//注：这里的函数windows和linux平台通用

//将数值转化为字符串
//std::tstring convertToString(int value);

//将数值转化为字符串，width指明宽度，不足的补0
//std::tstring convertToString(int value, int width);

template<class T> std::tstring convertToString(T value)
{
    std::tstringstream ss;
    ss << value;
    return ss.str();
}

template<class T> std::tstring convertToString(T value, int width)
{
    std::tstringstream ss;
#if defined(UNICODE) || defined(_UNICODE)
    ss << std::setw(width) << std::setfill(L'0') << value;
#else
    ss << std::setw(width) << std::setfill('0') << value;
#endif
    return ss.str();
}

template<class T> void convertFromString(T &value, const std::tstring &s)
{
    std::tstringstream ss(s);
    ss >> value;
}

//template<class T> T convertFromString(const std::tstring &s)
//{
//    T value;
//    std::tstringstream ss(s);
//    ss >> value;
//    return value;
//}

//得到计算机名
void globalGetComputerName();

//读取json文件中的项，并将其放置在一个列表中
bool globalGetParamFromConfigJson(std::tstring Path, std::map<std::tstring, std::tstring> &lst);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, std::tstring Value);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, int Value);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, UINT Value);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, INT64 Value);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, double Value);

//写入值到json文件
bool globalSetParamToConfigJson(std::tstring Path, std::tstring Name, Json::Value &Value);

//从json中删除指定项
bool globalDeleteParamFromConfigJson(std::tstring Path, std::tstring Name);

//将字符串转换为整型数
int globalStrToIntDef(const LPTSTR valuechar, int &value, int defaultvalue = -1, int base = 10);
int globalStrToInt(const LPTSTR valuechar, int defaultvalue = -1, int base = 10);

//创建一个GUID，不带分隔符，例如：781D9B347445425294F976FDE7AD5161
void globalCreateGUID(std::tstring &GuidStr);
//创建GUID，带分隔符，例如：5AB9B709-2BBE-CBFF-3EC3-61CD79F8339C
void globalCreateGUID_Line(std::tstring &GuidStr);

//检查GUID格式是否为{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}，注意x为数字或小写字母a到f
bool globalCheckGUID_One(const std::tstring &guidStr, std::tstring &err);

//获取文件名，例如输入C:\123.MXF,将返回123.MXF
std::tstring globalExtractFileName(const std::tstring &Path);

//获取文件后缀，例如输入C:\123.MXF,将返回.MXF
std::tstring globalExtractFileExt(const std::tstring &Path);

//获取文件路径，例如输入C:\123.MXF,将返回C:\值
std::tstring globalExtractFilePath(const std::tstring &Path);

//获取上一级文件路径，如果已经到根目录，返回false
bool globalExtractFatherPath(const std::tstring &Path, std::tstring &FatherPath);

//获取文件名--不带后缀，例如输入C:\123.MXF,将返回123
std::tstring globalExtractFileNameExcludeExt(const std::tstring &Path);

//替换后缀名，例如输入C:\123.MXF，想替换为.xml，将返回C:\123.xml
std::tstring globalChangeFileExt(const std::tstring &Path, const std::tstring &Ext);

//确保文件路径最后结尾包含斜杠，例如输入C:\Clips，返回C:\Clips\，如果输入C:\Clips\还是返回C:\Clips\ 如果是FTP路径，将在最后加入反斜杆，例如ftp://user:password@192.168.0.1/Clips，将返回ftp://user:password@192.168.0.1/Clips/
void globalIncludePathDelimiter(std::tstring &Path);

void globalIncludeUrlDelimiter(std::tstring &Url);

//确保文件路径格式正确，例如如果是：C:\Clips/，将返回C:\Clips\,如果是ftp路径，则保证都是反斜杆
void globalCorrectPath(std::tstring &Path);

//将一个用分隔的字符串解析到列表，例如：123|435|678，将返回一个列表，支持"|123|456|789"或"123|456|789|"等格式，分隔符默认为|
void globalSpliteString(const std::tstring &str, std::vector<std::tstring> &strVector, std::tstring splitStr = _T("|"), int maxCount = 1024);

//从一个共享路径截取IP地址或计算机名 -- 注：只有windows系统共享路径支持，linux暂时不支持
std::tstring globalGetHostByPath(const std::tstring &Path);

//获取当前执行文件的路径
std::tstring globalGetFilePath();

//创建文件夹 -- 该函数能保证上级目录不存在时也能创建成功
bool globalCreateDirectory(std::tstring Path);

//强制结束指定进程
bool globalTerminateProcess(std::tstring ProcessName);
bool globalTerminateProcess(int ProcessId);
bool globalTerminateProcess(std::vector<int> &vector, std::tstring &msg);

//判断指定进程是否在运行
bool globalIsProcessRun(std::tstring ProcessName);
bool globalIsProcessRun(int ProcessId);

//根据名称得到进程Pid
void globalGetProcessIdByName(std::tstring ProcessName, std::vector<int> &IdVector);

//删除指定的文件
bool globalDeleteFile(std::tstring strFileName);

//递归删除指定文件夹
bool globalDeleteFolder(std::tstring strFilePath, std::tstring &ErrorMsg, bool RemoveDirectoryFlag = true);

//获取指定目录下所有文件 Type含义 0-获取文件和文件夹 1-只获取所有子文件 2-只获取所有子文件夹，Ext表示只获取指定类型的文件，例如输入.json，将只获取该目录下所有的json文件
bool globalGetFolderFile(std::tstring strFilePath, int Type, std::vector<std::tstring> &FileVector, std::tstring &ErrorMsg, std::tstring Ext = _T(""));

//将一个字符串去除前面、后面以及前后的空格等多余字符
std::tstring &ltrim(std::tstring &s);
std::tstring &rtrim(std::tstring &s);
std::tstring &trim(std::tstring &s);

//Unicode和UTF8互转
void unicode_to_utf8(const wchar_t* in, size_t len, std::string& out);
void utf8_to_unicode(const char* in, size_t len, std::wstring& out);

//Unicode和Ansi互转
void ansi_to_unicode(const char* in, size_t len, std::wstring& out);
void unicode_to_ansi(const wchar_t* in, size_t len, std::string& out);

//Ansi和UTF8互转
void ansi_to_utf8(const char* in, size_t len, std::string& out);
void utf8_to_ansi(const char* in, size_t len, std::string& out);

//判断v4IP地址是否合法
bool globalIsIPStringValid(std::tstring IPString);

//判断子网掩码是否有效 -- 高位为连续的1，直到剩余的全为0，不能有断断续续的情况
bool globalIsMaskStringValid(std::tstring MaskString);

//判断组播地址是否合法,useFlag为false时判断所有组播地址范围，为true判断用户可用范围，组播地址范围从224.0.0.0到239.255.255.255，用户可用范围时224.0.2.0到238.255.255.255
bool globalIsMultiIpValid(std::tstring IPString, bool useFlag = false);

//将v4IP地址转换为整型值
INT64 globalGetIpByString(std::tstring IpAddr);

DWORD globalIP2DWORD(std::tstring IpAddr);//转换成网络能认的地址

//得到本机IP地址列表
void globalGetHostIpAddr(std::vector<std::tstring> &IpList);

//判断一个文件是否存在 -- 暂时不支持ftp路径的检查
bool globalFileExists(std::tstring FileName);

//判断一个文件夹是否存在，注：暂时还未实现ftp路径的检查
bool globalDirectoryExists(std::tstring Directory);

//得到文件大小，返回值为字节数，如果得到失败，返回0
INT64 globalGetFileSize(std::tstring FileName);

//将文件大小（字节数）转换为实际显示，例如：23.4MB
std::tstring globalFileSizeShow(INT64 &FileSize);

//大小写转换
std::tstring upperCase(const std::tstring &in);
std::tstring lowerCase(const std::tstring &in);

//将字符串中所有指定的字符串替换为新的字符串，例如replace(_T("AA-BB-CC-DD"), _T("-"), _T("_"))，将会返回"AA_BB_CC_DD"
std::tstring replace(const TCHAR *pszSrc, const TCHAR *pszOld, const TCHAR *pszNew);

//16进制数字转字符串 - 例如 0x21 转换为字符串 "21"，注：暂时只支持0x00-0xff之间的数
bool globalDecToHex(unsigned int idec, std::tstring &str);

//16进制字符串转数字 - 例如 "12" 转换成数字 0x12，注：最多8位，不要带0x开头
bool globalHexToDec(LPCTSTR shex, unsigned int& idec);

//将Buffer转换为十六进制的字符串，主要用于日志显示
void globalBufToShowString(LPBYTE buf, UINT size, std::tstring &showStr);

//将小数点后一位进行四舍五入后转换为整数
int globalRound(double value);

//将浮点数四舍五入并保留指定位数小数
double globalRound(double value, int precision);

//64位整数和字符串之间互转，注意：这里是十进制的
bool atoi64_t(char *arrTmp, INT64 &value);//字符串转INT64
bool atoiu64_t(char *arrTmp, UINT64 &value);//字符串转UINT64

bool itoa64_t(INT64 n, char *p, int len);//INT64转字符串
bool itoa64_t(INT64 n, std::tstring &str);//INT64转字符串

bool itoau64_t(UINT64 n, char *p, int len);//UINT64转字符串
bool itoau64_t(UINT64 n, std::tstring &str);//UINT64转字符串

//十六进制字符串转无符号的INT64，例如9a45ba37a，注意：全小写
UINT64 atoui64_t(const char* str);

//将一个时间点转换为字符串显示
std::tstring globalSerializeTimePoint( const std::chrono::system_clock::time_point &time, const std::string &format, bool bIncludeMS = true);

//将时间转换为本地时间使用
std::tm globalSystemClock2LocalTime(std::chrono::system_clock::time_point time);

//将格式为IP地址:端口的字符串进行解析，例如127.0.0.1:6379或http://225.0.0.1:1234这种格式或者snmp://127.0.0.1:161/?version=SNMPv2&read=pulbic&write=private这种格式
void globalResolveAddrStr(const std::tstring &str, std::tstring &protocol, std::tstring &addr, int &port, std::tstring &path, std::tstring &param, std::tstring &user, std::tstring &pwd);
//数据库字符串解析 如--db=mysql;uhdplayout;uhdplayout;192.168.56.128:1521/orcl
void globalResolveDbStr(const std::tstring &str, int &type, std::tstring &addr, std::tstring &dbname, int &port, std::tstring &user, std::tstring &pwd);

//输出时码显示，Value-帧数，frameRate-帧率
void globalPrintTimecode(TCHAR *out, int nBufferSize, DWORD Value, double frameRate);
void globalPrintTimecode(std::tstring &Str, DWORD Value, double frameRate);
//根据技审帧率序号得到实际帧率
double globalGetFrameRateByIndex(int index);
//得到当前的ptp时间
INT64 getSMPTE2059TAI_Time();
void getSMPTE2059TAI_Time(std::tstring &str, INT64 value = -1);
void getSMPTE2059TAI_Time(TCHAR *str, int len, INT64 value = -1);
//将格式为"秒:纳秒"格式字符串转换为ptp时间值
INT64 convertSMPTE2059TAI_Time(const std::tstring str);
