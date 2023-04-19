#include "StdAfx.h"
#include ".\driver.h"

CSockDriver& CSockDriver::Instance(void)
{
	static CSockDriver dl;
	return dl;
}

CSockDriver::CSockDriver(void)
{
}

CSockDriver::~CSockDriver(void)
{
	//ClearSoket();
}

int CSockDriver::AddSocket(SOCKET s, void* act, ISockHandler* pHandler, long lNetworkEvents)
{
	Guard guard(m_lock);

	if(pHandler == NULL || m_socketMap.find(s) != m_socketMap.end())
	{
		g_error.SetError(1, -1, 1u);
		return -1;
	}

	for(size_t i=0u; i<m_drivers.size(); i++)
	{
		CDriverThread* pDriver = m_drivers[i];
		if(!pDriver->IsSocketFull())
		{
			if(pDriver->AddSocket(s, act, pHandler, lNetworkEvents) == -1)
				return -1;
			m_socketMap.insert(std::make_pair(s, pDriver));
			return 0;
		}
	}

	CDriverThread* pDriver = new CDriverThread();
	if(pDriver->AddSocket(s, act, pHandler, lNetworkEvents) == -1)
	{
		delete pDriver;
		return -1;
	}
	else
	{
		m_drivers.push_back(pDriver);
		m_socketMap.insert(std::make_pair(s, pDriver));
	}

	return 0;
}

int CSockDriver::RemoveSoket(SOCKET s)
{
	Guard guard(m_lock);

	SocketMap::iterator iter = m_socketMap.find(s);
	if(iter == m_socketMap.end())
	{
		g_error.SetError(2, -1, 1u);
		return -1;
	}

	CDriverThread* pDriver = iter->second;
	if(pDriver->RemoveSoket(s) == -1)
		return -1;

	m_socketMap.erase(s);
	/*DriverVec::iterator pos = m_drivers.begin();
	for(; pos < m_drivers.end(); ++pos)
	{
		if(*pos == pDriver)
		{
			if(pDriver->IsSocketEmpty())
			{
				delete pDriver;
				m_drivers.erase(pos);
			}
			break;
		}
	}*/

	return 0;
}

int CSockDriver::ClearSoket(void)
{
	Guard guard(m_lock);

	m_socketMap.clear();
	for(size_t i=0u; i<m_drivers.size(); i++)
	{
		delete m_drivers[i];
	}
	m_drivers.clear();

	return 0;
}

DWORD CSockDriver::GetSockThreadID(SOCKET s)
{
	Guard guard(m_lock);

	SocketMap::iterator iter = m_socketMap.find(s);
	if(iter != m_socketMap.end())
	{
		return iter->second->GetThreadID();
	}

	return -1;
}