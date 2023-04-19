/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Driver.h
*
*  Driver.h,v 1.00 2006/12/23
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#pragma once
#include "isockdriver.h"
#include "DriverThread.h"

class CSockDriver :
	public ISockDriver
{
public:
	static CSockDriver& Instance(void);

	virtual int AddSocket(SOCKET s, void* act, ISockHandler* pHandler, long lNetworkEvents = FD_READ|FD_WRITE);
	virtual int RemoveSoket(SOCKET s);
	virtual int ClearSoket(void);
	virtual DWORD GetSockThreadID(SOCKET s);

protected:
	CSockDriver(void);
	~CSockDriver(void);

protected:
	typedef std::vector<CDriverThread*>			DriverVec;
	typedef std::map<SOCKET, CDriverThread*>	SocketMap;

	DriverVec	m_drivers;
	SocketMap	m_socketMap;
	Lock		m_lock;
};
