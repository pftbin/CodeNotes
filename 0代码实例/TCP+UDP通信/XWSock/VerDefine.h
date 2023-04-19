//VerDefine.h
#pragma once

#ifdef XP
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#define _WIN32_WINDOWS 0x0500
#endif

#ifdef VISTA
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define _WIN32_WINDOWS 0x0610
#define _WIN32_IE 0x600
#endif