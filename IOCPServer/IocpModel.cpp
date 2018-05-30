#include "IocpModel.h"

#ifdef __WIN__

CIocpModel::CIocpModel(const char* ip, unsigned short port):
							m_nThreads(0),
							m_hShutdownEvent(NULL),
							m_hIOCompletionPort(NULL),
							m_phWorkerThreads(NULL),
							m_strIP(ip),
							m_usPort(port),
							m_lpfnAcceptEx(NULL),
							m_lpfnGetAcceptExSocketAddr(NULL),
							m_bStart(false)
{
}

CIocpModel::~CIocpModel()
{
	Stop();
}

bool CIocpModel::Start()
{
	// ��ʼ��������
	InitializeCriticalSection(&m_csMutex);

	// ����ϵͳ�˳��¼�֪ͨ
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// ��ʼ��IOCP
	if (false == _InitializeIOCP()) 
	{
		m_log_cb("\n��ʼ��IOCPʧ��\n");
		return false;
	}
	else
	{
		m_log_cb("\nIOCP��ʼ�����\n");
	}

	// ��ʼ��Socket
	if (false == _InitializeListenSocket())
	{
		m_log_cb("\nListen Socket ��ʼ��ʧ��\n");
		_DeInitialize();
		return false;
	}
	else
	{
		m_log_cb("\nϵͳ׼������, �Ⱥ�����....\n");
	}

	m_bStart = true;

	return true;
}

bool CIocpModel::_InitializeIOCP()
{
	// ������һ����ɶ˿�
	m_hIOCompletionPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0);

	if (NULL == m_hIOCompletionPort)
	{
		m_log_cb("\n������ɶ˿�ʧ��! ������룺%d!\n", WSAGetLastError());
		return false;
	}

	// ���ݱ����еĴ�����������������Ӧ���߳�����CPU������*2 + 2.
	m_nThreads = 2 * _GetNoOfProcessors() + 2;

	// Ϊ�������̳߳�ʼ�����
	m_phWorkerThreads = new HANDLE[m_nThreads];

	//���ݼ�����������������������߳�
	DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		THREADPARAMS_WORKER* pThreadParams = new THREADPARAMS_WORKER;
		pThreadParams->nThreadNo	= i + 1;
		pThreadParams->pIOCPModel	= (void*)this;
		m_phWorkerThreads[i]		= CreateThread(
			0, 
			0, 
			_WorkerThread, 
			(void*)pThreadParams,
			0,
			&nThreadID);
	}
	
	return true;
}

bool CIocpModel::_InitializeListenSocket()
{
	// AcceptEx �� GetAcceptExSockaddrs ��GUID�����ڵ�������ָ��
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSocketAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// ��������ַ��Ϣ�����ڰ�Socket
	struct sockaddr_in ServerAddress;

	// �������ڼ�����Socket��Ϣ
	m_pListenContext = new PER_SOCKET_CONTEXT;

	// ��Ҫʹ���ص�IO�������ʹ��WSASocket������Socket���ſ���֧���ص�IO����
	m_pListenContext->m_Socket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		0,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_pListenContext->m_Socket)
	{
		return false;
	}

	// ��Listen Socket������ɶ˿���
	if (NULL == CreateIoCompletionPort(
		(HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort, (DWORD)m_pListenContext, 0))
	{
		delete m_pListenContext;
		m_pListenContext = NULL;
		return false;
	}

	// ����ַ��Ϣ
	ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	ServerAddress.sin_port = htons(m_usPort);
	
	// �󶨵�ַ�Ͷ˿�
	if (SOCKET_ERROR == bind(m_pListenContext->m_Socket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)))
	{
		return false;
	}

	return true;
}

void CIocpModel::_DeInitialize()
{

}

int CIocpModel::_GetNoOfProcessors()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

DWORD WINAPI CIocpModel::_WorkerThread(LPVOID lpParam)
{
	return 0;
}

void CIocpModel::Stop()
{

}

#endif