#ifndef _DBE_TASK_CPP_
#define _DBE_TASK_CPP_

#include <cassert>
#include <process.h>
#include "Task.h"

namespace dbe
{
	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Task ( typename Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::MessageQueue *pMsgQueue )
	: m_bDeleteMsgQueue(FALSE)
	, TaskCallBack(NULL)
	, m_pCallBackParam(NULL)
	, m_taskId(-1)
	, m_closeEvent(TRUE, TRUE)
	{
		if (pMsgQueue == NULL)
		{
			pMsgQueue = new Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::MessageQueue;
			m_bDeleteMsgQueue = TRUE;
		}

		m_pMsgQueue = pMsgQueue;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Task (size_t hwm, size_t lwm)
	: m_bDeleteMsgQueue(FALSE)
	, TaskCallBack(NULL)
	, m_pCallBackParam(NULL)
	, m_taskId(-1)
	, m_closeEvent(TRUE, TRUE)
	{
		m_pMsgQueue = new Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::MessageQueue(hwm, lwm);
		m_bDeleteMsgQueue = TRUE;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	~Task (void)
	{
		Close();

		if (m_bDeleteMsgQueue)
		{
			delete m_pMsgQueue;
			m_pMsgQueue = NULL;
			m_bDeleteMsgQueue = FALSE;
		}
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Open (size_t threadCount, 
		  TASKCALLBACK SvcCallBack, 
		  void* pParam1,
		  FLUSHCALLBACK FlushCallBack,
		  void* pParam2)
	{
		Guard guard(m_lock);

		if(!m_hThreads.empty() || threadCount == 0)
			return 0;

		assert(m_pMsgQueue != NULL);
		m_pMsgQueue->SetFlushCallBack(FlushCallBack, pParam2);
		m_pMsgQueue->Activate();
		m_hThreads.reserve(threadCount);
		TaskCallBack = SvcCallBack;
		m_pCallBackParam = pParam1;

		for(size_t i=0; i<threadCount; i++)
		{
			ThreadData* pData = new ThreadData;
			pData->pTask	= this;
			pData->hThread	= NULL;
			pData->bClosed	= FALSE;

			unsigned threadId;
			pData->hThread = (HANDLE)_beginthreadex(NULL, 0, SvcRun, pData, 0, &threadId);
			if(pData->hThread != NULL)
			{
				m_hThreads.push_back(pData);
			}
			else
			{
				delete pData;
			}
		}

		size_t count = m_hThreads.size();
		if(count > 0)
		{
			m_closeEvent.ResetEvent();
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Close (bool bTerminate, DWORD dwWaitTime)
	{
		Guard guard(m_lock);

		size_t threadCount = m_hThreads.size();

		if(threadCount == 0)
			return 0;

		assert(m_pMsgQueue != NULL);
		m_pMsgQueue->Close();

		if(!bTerminate)
		{
			for(size_t i=0; i<threadCount; i++)
			{
				m_hThreads[i]->bClosed = TRUE;
				WaitForSingleObject(m_hThreads[i]->hThread, INFINITE);
				CloseHandle(m_hThreads[i]->hThread);
				delete m_hThreads[i];
			}
		}
		else
		{
			for(size_t i=0; i<threadCount; i++)
			{
				m_hThreads[i]->bClosed = TRUE;
				if(WAIT_OBJECT_0 != WaitForSingleObject(m_hThreads[i]->hThread, dwWaitTime))
				{
					TerminateThread(m_hThreads[i]->hThread, -1);
				}
				CloseHandle(m_hThreads[i]->hThread);
				delete m_hThreads[i];
			}
		}

		m_hThreads.clear();
		m_closeEvent.SetEvent();

		return threadCount;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	AddThread(size_t threadCount)
	{
		Guard guard(m_lock);

		if(m_hThreads.empty())
			return 0;

		size_t count = 0;
		for(size_t i=0; i<threadCount; i++)
		{
			ThreadData* pData = new ThreadData;
			pData->pTask	= this;
			pData->hThread	= NULL;
			pData->bClosed	= FALSE;

			unsigned threadId;
			pData->hThread = (HANDLE)_beginthreadex(NULL, 0, SvcRun, pData, 0, &threadId);
			if(pData->hThread != NULL)
			{
				count++;
				m_hThreads.push_back(pData);
			}
			else
			{
				delete pData;
			}
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	SubThread(size_t threadCount)
	{
		Guard guard(m_lock);

		size_t thrSize = m_hThreads.size();

		if(threadCount == 0 || thrSize <= 1)
			return 0;

		if(threadCount > thrSize-1)
			threadCount = thrSize-1;

		for(size_t i=thrSize-threadCount; i<thrSize; i++)
		{
			m_hThreads[i]->bClosed = TRUE;
		}

		assert(m_pMsgQueue != NULL);
		m_pMsgQueue->Pulse();
	
		for(size_t i=0; i<threadCount; i++)
		{
			ThreadData* pData = m_hThreads.back();
			WaitForSingleObject(pData->hThread, INFINITE);
			CloseHandle(pData->hThread);
			delete pData;
			m_hThreads.pop_back();
		}

		m_pMsgQueue->Activate();

		return threadCount;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	BOOL Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	IsOpened (void)
	{
		Guard guard(m_lock);
		return (!m_hThreads.empty());
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Put (MESSAGE_TYPE& msg, DWORD timeout)
	{
		return m_pMsgQueue->Enqueue(msg, timeout);
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Unput (MESSAGE_TYPE& msg, DWORD timeout)
	{
		return m_pMsgQueue->DequeueTail(msg, timeout);
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Get (MESSAGE_TYPE& msg, DWORD timeout)
	{
		return m_pMsgQueue->Dequeue(msg, timeout);
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Unget (MESSAGE_TYPE& msg, DWORD timeout)
	{
		return m_pMsgQueue->EnqueueHead(msg, timeout);
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	BOOL Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Wait (DWORD timeout)
	{
		return m_closeEvent.Wait(timeout);
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Suspend (void)
	{
		Guard guard(m_lock);

		size_t count = 0;
		for(size_t i=0; i<m_hThreads.size(); i++)
		{
			if(::SuspendThread(m_hThreads[i]->hThread) != -1)
			{
				count++;
			}
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Resume (void)
	{
		Guard guard(m_lock);

		size_t count = 0;
		for(size_t i=0; i<m_hThreads.size(); i++)
		{
			if(::ResumeThread(m_hThreads[i]->hThread) != -1)
			{
				count++;
			}
		}

		return count;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	size_t Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	GetThreadCount (void)
	{
		Guard guard(m_lock);
		return m_hThreads.size();
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	typename Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	MessageQueue* Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	GetMessageQueue (void)
	{
		return m_pMsgQueue;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	BOOL Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	SetMessageQueue (typename Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::MessageQueue& msgQueue)
	{
		if(IsOpened()) return -1;
		if(m_bDeleteMsgQueue)
		{
			delete m_pMsgQueue;
			m_bDeleteMsgQueue = FALSE;
		}
		m_pMsgQueue = &msgQueue;
		return 0;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	void Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	TaskId (int taskId)
	{
		m_taskId = taskId;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	TaskId (void)
	{
		return m_taskId;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	int Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	Svc (const MESSAGE_TYPE& msg)
	{
		return -1;
	}

	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	unsigned __stdcall Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>::
	SvcRun (void* pParam)
	{
		ThreadData* pData = (ThreadData*)pParam;
		Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY> *pTask = pData->pTask;
		assert(pTask != NULL && pTask->m_pMsgQueue != NULL);

		if(pTask->TaskCallBack == NULL)
		{
			while(!pData->bClosed)
			{
				MESSAGE_TYPE msg;
				if(pTask->Get(msg, INFINITE) != -1)
				{
					if(pTask->Svc(msg) == -1)
					{
						return -1;
					}
				}
			}
		}
		else
		{
			while(!pData->bClosed)
			{
				MESSAGE_TYPE msg;
				if(pTask->Get(msg, INFINITE) != -1)
				{
					if(pTask->TaskCallBack(pTask->m_pCallBackParam, msg) == -1)
					{
						return -1;
					}
				}
			}
		}

		return 0;
	}
}

#endif