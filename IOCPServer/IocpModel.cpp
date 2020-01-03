#include "IocpModel.h"
#include <iostream>
using namespace std;
#ifdef __WIN__


// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define MAX_POST_ACCEPT              10
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

static void log_(const char* format, ...)
{
	static char info[512];
	va_list list;
	va_start( list, format );
	vsprintf( info, format, list );
	va_end( list );
	printf("%s\n", info);
}


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
	m_log_cb = log_;
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
		m_log_cb("\nIOCP初始化完毕%d\n",2);
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

void *CIocpModel::_GetExtenSionFun(SOCKET s, const GUID *which_fn)
{
	void *ptr = NULL;
	DWORD bytes=0;
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
	    (GUID*)which_fn, sizeof(*which_fn),
	    &ptr, sizeof(ptr),
	    &bytes, NULL, NULL);
	return ptr;
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

	if (SOCKET_ERROR == listen(m_pListenContext->m_Socket, SOMAXCONN))
	{
		return false;
	}


	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
	m_lpfnAcceptEx = (LPFN_ACCEPTEX)_GetExtenSionFun(m_pListenContext->m_Socket, &GuidAcceptEx);

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	m_lpfnGetAcceptExSocketAddr = (LPFN_GETACCEPTEXSOCKADDRS)_GetExtenSionFun(m_pListenContext->m_Socket, &GuidGetAcceptExSocketAddrs);

	for (int i = 0; i < MAX_POST_ACCEPT; ++i)
	{
		PER_IO_CONTEXT* pAcceptIoContext = m_pListenContext->GetNewIoContext();
		if (false == _PostAccept(pAcceptIoContext))
		{
			m_pListenContext->RemoveContext(pAcceptIoContext);
			return false;
		}
	}

	return true;
}

void CIocpModel::_DeInitialize()
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

	// 关闭监听Socket
	RELEASE(m_pListenContext);
}

int CIocpModel::_GetNoOfProcessors()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

DWORD WINAPI CIocpModel::_WorkerThread(LPVOID lpParam)
{
	THREADPARAMS_WORKER* pParam = (THREADPARAMS_WORKER*)lpParam;
	CIocpModel* pIocpModel = (CIocpModel*)pParam->pIOCPModel;
	int iThreadNo = (int)pParam->nThreadNo;

	printf("工作者线程启动，ID：%d.\n", iThreadNo);

	OVERLAPPED				*pOverlaped			= NULL;
	PER_SOCKET_CONTEXT		*pSocketContext		= NULL;
	DWORD					dwBytesTransfered	= 0;

	while (WAIT_OBJECT_0 != WaitForSingleObject(pIocpModel->m_hShutdownEvent, 0))
	{
		bool bReturn = GetQueuedCompletionStatus(
			pIocpModel->m_hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&pSocketContext,
			&pOverlaped,
			INFINITE
		);

		if (EXIT_CODE == (DWORD)pSocketContext)
		{
			cout << "EXIT_CODE.." << endl;
			break;
		}

		if (!bReturn)
		{
			DWORD dwErr = GetLastError();
			cout << "!bReturn.." << endl;
			// 非超时 && 非网络断开
			if (WAIT_TIMEOUT != dwErr && ERROR_NETNAME_DELETED != dwErr)
			{
				break;
			}
			continue;
		}
		else
		{
			PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlaped, PER_IO_CONTEXT, m_Overlapped);

			//断开连接
			if ((0 == dwBytesTransfered) && (POSTED_RECV == pIoContext->m_OpType || POSTED_SEND == pIoContext->m_OpType))
			{
				cout << pSocketContext->m_strIP.c_str() << endl;
				cout << pSocketContext->m_usPort << endl;
				cout << "断开连接..." << endl;
				continue;
			}
			else
			{
				switch(pIoContext->m_OpType)
				{
				case POSTED_ACCEPT:
					{
						pIocpModel->_DoAccpet(pSocketContext, pIoContext);
					}
					break;
				case POSTED_RECV:
					{
						pIocpModel->_DoRecv(pSocketContext, pIoContext);
					}
					break;
				case POSTED_SEND:
					{
						pIocpModel->_DoSend(pSocketContext, pIoContext);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return 0;
}

void CIocpModel::Stop()
{
	if (m_pListenContext != NULL && m_pListenContext->m_Socket != INVALID_SOCKET)
	{
		//激活关闭消息通知
		m_log_cb("激活关闭消息通知");
		SetEvent(m_hShutdownEvent);
		
		for (int i = 0; i < m_nThreads; i++)
		{
			m_log_cb("通知所有完成端口操作退出");
			//通知所有完成端口操作退出
			PostQueuedCompletionStatus(
					m_hIOCompletionPort,
					0,
					(DWORD)EXIT_CODE,
					NULL
				);
		}

		m_log_cb("等待所有的客户端资源退出");
		// 等待所有的客户端资源退出
		WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

		// 清除客户端列表信息
		
	
		m_log_cb("释放其他资源");
		// 释放其他资源
		_DeInitialize();
	}
}

bool CIocpModel::_PostAccept(PER_IO_CONTEXT* pAcceptIoContext)
{
	if (pAcceptIoContext->m_sockAccept != INVALID_SOCKET) 
	{
		return false;
	}

	DWORD dwBytes = 0;
	pAcceptIoContext->m_OpType = POSTED_ACCEPT;
	WSABUF *pwBuf = &pAcceptIoContext->m_wsaBuf;

	pAcceptIoContext->m_sockAccept = WSASocket(
			AF_INET,
			SOCK_STREAM,
			IPPROTO_TCP,
			NULL,
			0,
			WSA_FLAG_OVERLAPPED
		);
	if (INVALID_SOCKET == pAcceptIoContext->m_sockAccept)
	{
		return false;
	}
	pAcceptIoContext->m_wsaBuf.len = ((sizeof(SOCKADDR_IN) + 16) * 2) + 1;

	if (false == m_lpfnAcceptEx(
								m_pListenContext->m_Socket,
								pAcceptIoContext->m_sockAccept,
								pAcceptIoContext->m_wsaBuf.buf,
								pAcceptIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
								sizeof(SOCKADDR_IN) + 16,
								sizeof(SOCKADDR_IN) + 16,
								&dwBytes,
								&(pAcceptIoContext->m_Overlapped)))
	{
		DWORD dwErr = WSAGetLastError();
		if (WSA_IO_PENDING != dwErr)
		{
			return false;
		}
	}
	return true;
}

bool CIocpModel::_PostRecv( PER_IO_CONTEXT* pIoContext )
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	pIoContext->ResetBuffer();
	pIoContext->m_OpType = POSTED_RECV;

	int iBytesRecv = WSARecv(
		(pIoContext->m_sockAccept),
		&(pIoContext->m_wsaBuf),
		1,
		&dwBytes,
		&dwFlags,
		&(pIoContext->m_Overlapped),
		NULL
	);

	if ((SOCKET_ERROR == iBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}
	return true;
}

bool CIocpModel::_PostSend( PER_IO_CONTEXT* pIoContext )
{
	return true;
}

bool CIocpModel::_DoAccpet(PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext)
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr  = NULL;
	int iRemoteLen = sizeof(SOCKADDR_IN);
	int iLocalLen  = sizeof(SOCKADDR_IN);

	m_lpfnGetAcceptExSocketAddr(
		pIoContext->m_wsaBuf.buf,
		pIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		(LPSOCKADDR*)&LocalAddr,
		&iLocalLen,
		(LPSOCKADDR*)&ClientAddr,
		&iRemoteLen
	);

	PER_SOCKET_CONTEXT* pNewSocketContext	= new PER_SOCKET_CONTEXT;
	pNewSocketContext->m_Socket				= pIoContext->m_sockAccept;
	memcpy(&(pNewSocketContext->m_ClientAddr), ClientAddr, sizeof(SOCKADDR_IN));
	pNewSocketContext->m_strIP				= inet_ntoa(ClientAddr->sin_addr);
	pNewSocketContext->m_usPort				= ntohs(ClientAddr->sin_port);

	cout << pNewSocketContext->m_strIP.c_str() << endl;
	cout << pNewSocketContext->m_usPort << endl;
	cout << pIoContext->m_wsaBuf.buf << endl;

	PER_IO_CONTEXT* pNewIoContext	= pNewSocketContext->GetNewIoContext();
	pNewIoContext->m_OpType			= POSTED_SEND;
	pNewIoContext->m_sockAccept		= pNewSocketContext->m_Socket;

	//////////要传送的网页内容
	char CustomHtml[]=
		"<html>\r\n"
		"<head>\r\n"
		"<title></title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<p align=/\"center/\">欢迎访问我的主页</p>\r\n"
		"<h3 align=/\"center/\"><a href=Default.htm>快乐天地</a></h3>\r\n"
		"<p>结束</p>\r\n"
		"<p>　</p>\r\n"
		"</body></html>\r\n\r\n";
	/////////头部格式
	char hdrFmt[]=
		"HTTP/1.0 200 OK\r\n"
		"Server: MySocket Server\r\n"
		"Date: %s\r\n"
		"Content-Type: text/html\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: %d\r\n\r\n%s";
	char * strGmtNow="16/07/14 09:31:00 GMT";///////////随便指定了一个时间
	sprintf(pNewIoContext->m_wsaBuf.buf, hdrFmt, (const char*) strGmtNow, strlen(CustomHtml), CustomHtml);

	cout << pNewIoContext->m_wsaBuf.buf << endl;

	DWORD NumberOfBytesSent = 0;
	DWORD dwFlags = 0;
	WSASend(pNewIoContext->m_sockAccept,
		&(pNewIoContext->m_wsaBuf),
		1,
		&NumberOfBytesSent,
		dwFlags,
		&(pNewIoContext->m_Overlapped),
		NULL);

	PER_IO_CONTEXT* pNewIoContext_	= pNewSocketContext->GetNewIoContext();
	pNewIoContext_->m_OpType			= POSTED_RECV;
	pNewIoContext_->m_sockAccept		= pNewSocketContext->m_Socket;
	pNewIoContext_->ResetBuffer();

	if (false == _PostRecv(pNewIoContext_))
	{
		pNewSocketContext->RemoveContext(pNewIoContext_);
		return false;
	}
	
	pIoContext->ResetBuffer();
	return _PostAccept(pIoContext);

}

bool CIocpModel::_DoRecv( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext )
{
	return true;
}

bool CIocpModel::_DoSend( PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext )
{
	cout << "_DoSend.." << endl;
	return true;
}

bool CIocpModel::_AssociateWithIOCP(PER_SOCKET_CONTEXT *pContext)
{
	HANDLE hTemp = CreateIoCompletionPort(
		(HANDLE)pContext->m_Socket,
		m_hIOCompletionPort,
		(DWORD)pContext,
		0
	);

	if (NULL == hTemp)
	{
		return false;
	}

	return true;
}

#endif