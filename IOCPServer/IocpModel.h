#ifndef _IOCP_MODEL_H_
#define _IOCP_MODEL_H_

#ifdef WIN32
#include "config_win32.h"
#endif

#include <vector>
#include <algorithm>

#ifdef __WIN__

#include <winsock2.h>
#include <MSWSock.h>


// 缓冲区长度 (1024 * 8)
#define MAX_BUFFER_LEN        8192 

// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE
{
	POSTED_ACCEPT,				// 标志投递的Accept操作
	POSTED_SEND,				// 标志投递的是发送操作
	POSTED_RECV,				// 标志投递的是接收操作
	POSTED_NULL					// 用于初始化，无意义
}OPERATION_TYPE;


//单IO数据结构体定义(用于每一个重叠操作的参数)
typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED		m_Overlapped;					// 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)
	SOCKET			m_sockAccept;					// 这个网络操作所使用的Socket
	WSABUF			m_wsaBuf;						// WSA类型的缓冲区，用于给重叠操作传参数的
	char			m_szBuffer[MAX_BUFFER_LEN];		// 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE	m_OpType;						// 标识网络操作的类型(对应上面的枚举)
	bool			m_busy;							//标识该IO操作是否处于忙碌中，true为忙绿中。主要用于发送数据的判断

	// 初始化
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType     = POSTED_NULL;
		m_busy       = false;
	}

	// 释放掉Socket
	~_PER_IO_CONTEXT()
	{
		if(m_sockAccept != INVALID_SOCKET)
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}

	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}
}PER_IO_CONTEXT, *PPER_IO_CONTEXT;


//单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)
typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET							m_Socket;			// 每一个客户端连接的Socket
	SOCKADDR_IN						m_ClientAddr;		// 客户端的地址
	std::string						m_strIP;			// 客户端的IP地址
	unsigned short					m_usPort;			// 客户端的端口号
	std::vector<_PER_IO_CONTEXT*>	m_arrayIoContext;	// 客户端网络操作的上下文数据，
														// 也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的
	// 初始化
	_PER_SOCKET_CONTEXT()
	{
		m_Socket	= INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
		m_usPort	= 0;
	}

	// 释放资源
	~_PER_SOCKET_CONTEXT()
	{
		if (m_Socket != INVALID_SOCKET) 
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}

		for (std::vector<_PER_IO_CONTEXT*>::iterator it = m_arrayIoContext.begin(); it != m_arrayIoContext.end(); ++it)
		{
			_PER_IO_CONTEXT* pIoContext = *it;
			if (pIoContext) {
				delete pIoContext;
				pIoContext = NULL;
			}
		}
		m_arrayIoContext.clear();
	}

	// 获取一个新的pIoContext
	_PER_IO_CONTEXT* GetNewIoContext()
	{
		_PER_IO_CONTEXT* pIoContext = new _PER_IO_CONTEXT;
		m_arrayIoContext.push_back(pIoContext);
		return pIoContext;
	}

	// 从数组中移除一个指定的pIoContext
	void RemoveContext(_PER_IO_CONTEXT* pIoContext)
	{
		if (NULL == pIoContext) 
		{
			return;
		}
		std::vector<_PER_IO_CONTEXT*>::iterator it = std::find(m_arrayIoContext.begin(), m_arrayIoContext.end(), pIoContext);
		if (it != m_arrayIoContext.end())
		{
			delete pIoContext;
			pIoContext = NULL;
			m_arrayIoContext.erase(it);
		}
	}

}PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;


//工作线程都是静态函数（线程函数是静态函数）
typedef struct _THREADPARAMS_WORKER
{
	void*		pIOCPModel;                                   // 类指针，用于调用类中的函数
	int         nThreadNo;                                    // 线程编号

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER; 

typedef void (*log)(const char* format, ...);

class CIocpModel
{
public:
	CIocpModel(const char* ip, unsigned short port);
	~CIocpModel();
public:
	
	//启动服务
	bool Start();

	// 停止服务
	void Stop();

protected:

	// 初始化IOCP
	bool _InitializeIOCP();

	// 初始化Socket
	bool _InitializeListenSocket();

	// 最后释放资源
	void _DeInitialize();

	// 获得本机的处理器数量
	int _GetNoOfProcessors();

	// 静态 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);
private:
	HANDLE								m_hShutdownEvent;				// 用来通知线程系统退出的事件，为了能够更好的退出线程

	HANDLE								m_hIOCompletionPort;			// 完成端口的句柄

	HANDLE*								m_phWorkerThreads;				// 工作者线程的句柄指针数组

	int									m_nThreads;						// 生成的线程数量

	std::string							m_strIP;						// 服务器端的IP地址

	unsigned short						m_usPort;						// 服务器端的监听端口

	CRITICAL_SECTION					m_csMutex;						// 用于Worker线程同步的互斥量

	bool								m_bStart;						

	LPFN_ACCEPTEX						m_lpfnAcceptEx;					// AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	
	LPFN_GETACCEPTEXSOCKADDRS			m_lpfnGetAcceptExSocketAddr;

	std::vector<PPER_SOCKET_CONTEXT>	m_arrayClientContext;			// 客户端Socket的Context信息

	PPER_SOCKET_CONTEXT					m_pListenContext;				// 用于监听的Socket的Context信息

	log									m_log_cb;
};

#endif

#endif