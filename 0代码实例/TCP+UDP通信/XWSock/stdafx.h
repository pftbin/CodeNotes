// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#include "VerDefine.h"
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
#include <winsock2.h>
#include <tchar.h>
#include "dbe\synch.h"
//#include "Loger.h"
#include "Error.h"
#pragma comment(lib, "ws2_32.lib")

extern CError g_error;

typedef dbe::ThreadSynch::Lock			Lock;
typedef dbe::ThreadSynch::Guard			Guard;
typedef dbe::ThreadSynch::UnGuard		UnGuard;
typedef dbe::ThreadSynch::RWLock		RWLock;
typedef dbe::ThreadSynch::ReadGuard		ReadGuard;
typedef dbe::ThreadSynch::WriteGuard	WriteGuard;
typedef dbe::ThreadSynch::ReadUnGuard	ReadUnGuard;
typedef dbe::ThreadSynch::WriteUnGuard	WriteUnGuard;

//DECLARE_LOGER(NetLoger)