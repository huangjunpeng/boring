#include "Iocp.h"

// ���ݸ�Worker�̵߳��˳��ź�
#define EXIT_CODE                    NULL
// �ͷ�ָ���
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// �ͷ�����ָ���
#define RELEASE_ARRAY(x)				{if(x != NULL){delete[] x; x = NULL;}}
// �ͷž����
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// �ͷ�Socket��
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
	// ��ʼ��������
	InitializeCriticalSectionAndSpinCount(&m_csMutex, 1000);

	// ����ϵͳ�˳��¼�֪ͨ
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// ��ʼ��IOCP
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
		//֪ͨ������ɶ˿ڲ����˳�
		PostQueuedCompletionStatus(
			m_hIOCompletionPort,
			0,
			(DWORD)EXIT_CODE,
			NULL
			);
	}

	// �ȴ����еĿͻ�����Դ�˳�
	WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

	// �ͷ�������Դ
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
	// ������һ����ɶ˿�
	m_hIOCompletionPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0);

	if (NULL == m_hIOCompletionPort)
	{
		return false;
	}

	// Ϊ�������̳߳�ʼ�����
	m_phWorkerThreads = new HANDLE[m_nThreads];

	//���ݼ�����������������������߳�
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
	// ɾ���ͻ����б�����
	DeleteCriticalSection(&m_csMutex);

	// �ر�ϵͳ�˳��¼����
	RELEASE_HANDLE(m_hShutdownEvent);

	// �ͷŹ������߳̾��ָ��
	for (int i = 0; i < m_nThreads; i++)
	{
		RELEASE_HANDLE(m_phWorkerThreads[i]);
	}

	RELEASE_ARRAY(m_phWorkerThreads);

	// �ر�IOCP���
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
