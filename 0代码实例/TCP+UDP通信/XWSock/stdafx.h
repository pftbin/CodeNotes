// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

#include "VerDefine.h"
#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
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