/* -*- C++ -*- */

//=============================================================================
/**
*  @file    ISockDriver.h
*
*  ISockDriver.h,v 1.00 2006/12/23
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#ifndef __ISOCKDRIVER_H__
#define __ISOCKDRIVER_H__

class ISockHandler
{
public:
	virtual void HandleAccept(SOCKET s, void* act){}
	virtual void HandleConnect(SOCKET s, void* act, BOOL bConnected){}
	virtual void HandleInput(SOCKET s, void* act){}
	virtual void HandleOutput(SOCKET s, void* act){}
	virtual void HandleClose(SOCKET s, void* act){}
};

class ISockDriver
{
public:
	virtual int AddSocket(SOCKET s, void* act, ISockHandler* pHandler, long lNetworkEvents = FD_READ|FD_WRITE) = 0;
	virtual int RemoveSoket(SOCKET s) = 0;
	virtual int ClearSoket(void) = 0;
};

#endif