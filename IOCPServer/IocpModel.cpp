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
	// 初始化互斥量
	InitializeCriticalSection(&m_csMutex);

	// 建立系统退出事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	if (false == _InitializeIOCP()) 
	{
		m_log_cb("\n初始化IOCP失败\n");
		return false;
	}
	else
	{
		m_log_cb("\nIOCP初始化完毕\n");
	}

	// 初始化Socket
	if (false == _InitializeListenSocket())
	{
		m_log_cb("\nListen Socket 初始化失败\n");
		_DeInitialize();
		return false;
	}
	else
	{
		m_log_cb("\n系统准备就绪, 等候连接....\n");
	}

	m_bStart = true;

	return true;
}

bool CIocpModel::_InitializeIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0);

	if (NULL == m_hIOCompletionPort)
	{
		m_log_cb("\n建立完成端口失败! 错误代码：%d!\n", WSAGetLastError());
		return false;
	}

	// 根据本机中的处理器数量，建立对应的线程数，CPU核心数*2 + 2.
	m_nThreads = 2 * _GetNoOfProcessors() + 2;

	// 为工作者线程初始化句柄
	m_phWorkerThreads = new HANDLE[m_nThreads];

	//根据计算出来的数量建立工作者线程
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
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSocketAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;

	// 生成用于监听的Socket信息
	m_pListenContext = new PER_SOCKET_CONTEXT;

	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
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

	// 将Listen Socket绑定至完成端口中
	if (NULL == CreateIoCompletionPort(
		(HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort, (DWORD)m_pListenContext, 0))
	{
		delete m_pListenContext;
		m_pListenContext = NULL;
		return false;
	}

	// 填充地址信息
	ZeroMemory((char*)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.c_str());
	ServerAddress.sin_port = htons(m_usPort);
	
	// 绑定地址和端口
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