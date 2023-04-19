/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Task.h
*
*  Task.h,v 1.00 2006/01/05
*
*  @author л�� <xiewen@163.com>
*/
//=============================================================================

#ifndef _DBE_TASK_HEAD_
#define _DBE_TASK_HEAD_
#pragma once
#include <vector>
#include "MessageQueue.h"


namespace dbe
{
	template <class MESSAGE_TYPE, class DESTRUCTOR_TYPE, class SYNCH_STRATEGY>
	class Task
	{
	public:
		typedef int (*FLUSHCALLBACK)(void* pParam, MESSAGE_TYPE msgs[], size_t nCount);
		typedef int (__stdcall *TASKCALLBACK)(void* pParam, MESSAGE_TYPE& msg);
		typedef MessageQueue<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY> MessageQueue;

		/// Constructor.
		Task (MessageQueue *pMsgQueue = NULL);
		Task (size_t hwm, size_t lwm);

		/// Destructor.
		virtual ~Task (void);

		/**
		* �򿪸� Task��
		*
		* @param threadCount ϣ���������߳�����
		* @param SvcCallBack ����ص����������ΪNULL�������Svc()����
		* @param pParam      �����ص������Ĳ���
		*
		* @retval >0 �������߳�����
		* @retval -1 ʧ�ܣ������һ���Ѿ��򿪵�Task�ϵ��ø÷������ʧ�ܡ�
		*/
		virtual size_t Open (size_t threadCount = 1, 
							 TASKCALLBACK SvcCallBack = NULL,
							 void* pParam1 = NULL,
							 FLUSHCALLBACK FlushCallBack = NULL,
							 void* pParam2 = NULL);

		/**
		* �رո� Task��
		*
		* @retval �ر�ǰTaskӵ�е��߳�����
		*/
		virtual size_t Close (bool bTerminate = true, DWORD dwWaitTime = 1000);

		/**
		* �����̣߳�Task�����е��߳���Ŀ<1������µ��û�ʧ�ܣ���Task������
		* �Ѿ���Open�ġ�
		*
		* @param threadCount ϣ�����ӵ��߳�����
		*
		* @retval ʵ�����ӵ��߳�����
		*/
		virtual size_t AddThread(size_t threadCount = 1);

		/**
		* �����̣߳�Task�����е��߳���Ŀ<=1������µ��û�ʧ�ܣ����÷�������
		* �������е��߳���������Ϊ�㡣
		*
		* @param threadCount ϣ�����ٵ��߳�����
		*
		* @retval ʵ�ʼ��ٵ��߳�����
		*/
		virtual size_t SubThread(size_t threadCount = 1);

		/// ��Task�Ƿ��Ѿ�����
		virtual BOOL IsOpened (void);

		/// �������������յ�����ʱ����Task�е��̵߳��ȸ÷�����ǰ���ǵ���
		/// Open()����ʱSvcCallBack����ΪNULL���������-1�������߳̽��˳���
		virtual int Svc (const MESSAGE_TYPE& msg);

		/**
		* ����һ����Ϣ��Task�С�
		*
		* @param msg     ���뵽Task�е���Ϣ
		* @param timeout ��ʱʱ�䡣���Task�еĶ������������������
		*
		* @retval >0 �������Ϣ��Task�е���Ϣ��Ŀ��
		* @retval -1 ʧ�ܣ�����δ�������Ϣ������������
		*/
		virtual int Put (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* ��Taskβ��ȡ��һ����Ϣ��
		*
		* @param msg     ����ȡ������Ϣ
		* @param timeout ��ʱʱ�䡣���Task�еĶ���Ϊ�գ����������
		*
		* @retval >0 ȡ����Ϣ��Task�е���Ϣ��Ŀ��
		* @retval -1 ʧ�ܣ����ܶ����Ѿ�Ϊ�ա�
		*/
		virtual int Unput (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* ��Task��ȡ��һ����Ϣ
		*
		* @param msg     ����ȡ������Ϣ
		* @param timeout ��ʱʱ�䡣���Task�еĶ���Ϊ�գ����������
		*
		* @retval >0 ȡ����Ϣ��Task�е���Ϣ��Ŀ��
		* @retval -1 ʧ�ܣ����ܶ����Ѿ�Ϊ�ա�
		*/
		virtual int Get (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* ����һ����Ϣ��Taskͷ����
		*
		* @param msg     ����Task�е���Ϣ
		* @param timeout ��ʱʱ�䡣���Task�еĶ������������������
		*
		* @retval >0 �������Ϣ��Task�е���Ϣ��Ŀ��
		* @retval -1 ʧ�ܣ�����δ�������Ϣ������������
		*/
		virtual int Unget (MESSAGE_TYPE& msg, DWORD timeout = INFINITE);

		/**
		* �ȴ�Task�����رա�
		* 
		* @param timeout ��ʱʱ�䡣
		*
		* @retval true  Task�ڳ�ʱʱ���������رա�
		* @retval false Task��δ�رգ���ʱ��
		*/
		virtual BOOL Wait (DWORD timeout = INFINITE);

		/**
		* ����Task�е������߳�
		
		* @retval ������߳���Ŀ��
		*/
		virtual size_t Suspend (void);

		/**
		* ����Task�����б�������߳�
		*
		* @retval �������߳���Ŀ��
		*/
		virtual size_t Resume (void);

		/// ����Task�е��߳���Ŀ
		size_t GetThreadCount (void);

		/// ����Task����Ϣ����
		MessageQueue* GetMessageQueue (void);

		/// ����Task����Ϣ���У����Task�Ѿ��򿪣��������ʧ�ܡ�
		BOOL SetMessageQueue (MessageQueue& msgQueue);

		/// ���ñ�ʶ��Task��ID
		void TaskId (int taskId);

		/// ���ر�ʶ��Task��ID
		int TaskId (void);

	protected:
		Task(const Task&){}
		Task& operator=(const Task&){}
		static unsigned __stdcall SvcRun (void* pParam);

	protected:
		typedef typename SYNCH_STRATEGY::Lock  Lock;
		typedef typename SYNCH_STRATEGY::Guard Guard;
		typedef typename SYNCH_STRATEGY::Event Event;

	protected:
		struct ThreadData
		{
			Task<MESSAGE_TYPE, DESTRUCTOR_TYPE, SYNCH_STRATEGY>* pTask;
			HANDLE	hThread;
			BOOL	bClosed;
		};

	protected:
		MessageQueue*				m_pMsgQueue;
		BOOL						m_bDeleteMsgQueue;
		std::vector<ThreadData*>	m_hThreads;
		TASKCALLBACK				TaskCallBack;
		void*						m_pCallBackParam;
		int							m_taskId;
		Lock						m_lock;
		Event						m_closeEvent;
	};
}

#include "Task.inl"

#endif