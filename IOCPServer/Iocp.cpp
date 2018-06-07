#include "Iocp.h"

// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL
// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放数组指针宏
#define RELEASE_ARRAY(x)				{if(x != NULL){delete[] x; x = NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

CIocp::CIocp(int nThreads) :
								m_hIOCompletionPort(NULL),
								m_phWorkerThreads(NULL),
								m_hShutdownEvent(NULL),
								m_nThreads(nThreads),
								m_bStart(false)
{
}

CIocp::~CIocp()
{
}

bool CIocp::Start()
{
	// 初始化互斥量
	InitializeCriticalSectionAndSpinCount(&m_csMutex, 1000);

	// 建立系统退出事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	if (false == _InitializeIOCP()) 
	{
		return false;
	}

	m_bStart = true;

	return true;

}

void CIocp::Stop()
{
	SetEvent(m_hShutdownEvent);

	for (int i = 0; i < m_nThreads; i++)
	{
		//通知所有完成端口操作退出
		PostQueuedCompletionStatus(
			m_hIOCompletionPort,
			0,
			(DWORD)EXIT_CODE,
			NULL
			);
	}

	// 等待所有的客户端资源退出
	WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

	// 释放其他资源
	_DeInitialize();
}

bool CIocp::_AssociateWithIOCP(SOCKET fd, uintptr_t key)
{
	HANDLE h;
	h = CreateIoCompletionPort((HANDLE)fd, m_hIOCompletionPort, key, m_nThreads);
	if (!h)
		return false;
	return true;
}

bool CIocp::_InitializeIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0);

	if (NULL == m_hIOCompletionPort)
	{
		return false;
	}

	// 为工作者线程初始化句柄
	m_phWorkerThreads = new HANDLE[m_nThreads];

	//根据计算出来的数量建立工作者线程
	DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		m_phWorkerThreads[i]		= CreateThread(
			0, 
			0, 
			_WorkerThread, 
			(void*)this,
			0,
			&nThreadID);
	}

	return true;
}

void CIocp::_DeInitialize()
{
	// 删除客户端列表互斥量
	DeleteCriticalSection(&m_csMutex);

	// 关闭系统退出事件句柄
	RELEASE_HANDLE(m_hShutdownEvent);

	// 释放工作者线程句柄指针
	for (int i = 0; i < m_nThreads; i++)
	{
		RELEASE_HANDLE(m_phWorkerThreads[i]);
	}

	RELEASE_ARRAY(m_phWorkerThreads);

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);
}

DWORD WINAPI CIocp::_WorkerThread(LPVOID lpParam)
{
	CIocp* pIocp = (CIocp*)lpParam;
	OVERLAPPED	*pOverlaped			= NULL;
	DWORD		dwBytesTransfered	= 0;
	ULONG_PTR	key = 0;
	while (WAIT_OBJECT_0 != WaitForSingleObject(pIocp->m_hShutdownEvent, 0))
	{
		bool bReturn = GetQueuedCompletionStatus(pIocp->m_hIOCompletionPort, &dwBytesTransfered, &key, &pOverlaped, INFINITE);
		if (EXIT_CODE == (DWORD)key)
		{
			return 0;
		}		
		else
		{
			IocpOverlapped* eo = CONTAINING_RECORD(pOverlaped, IocpOverlapped, overlapped);
			eo->cb(eo, key, dwBytesTransfered, bReturn);
		}
	}
	return 0;
}
