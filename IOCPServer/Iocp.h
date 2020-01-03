#ifndef _IOCP_INCLUDE_
#define _IOCP_INCLUDE_

//超时
#define EV_TIMEOUT	0x01
//可读
#define EV_READ		0x02
//可写
#define EV_WRITE	0x04
//连接
#define EV_ACCEPT	0x08

#ifdef WIN32
#include "config_win32.h"
#endif

#ifdef __WIN__
#include <winsock2.h>

struct IocpOverlapped_;


typedef void (*IcopCallback)(IocpOverlapped_ *, SOCKET, DWORD, bool success);

typedef struct IocpOverlapped_ {
	OVERLAPPED overlapped;
	IcopCallback cb;
	void* data;
}IocpOverlapped, *LPIocpOverlapped;




class CIocp
{
public:
	CIocp(int nThreads);
	~CIocp();

	//启动服务
	bool Start();

	// 停止服务
	void Stop();

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP(SOCKET fd, uintptr_t key);

protected:
	// 初始化IOCP
	bool _InitializeIOCP();

	// 最后释放资源
	void _DeInitialize();

	// 静态 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);
private:
	HANDLE				m_hShutdownEvent;				// 用来通知线程系统退出的事件，为了能够更好的退出线程
	HANDLE				m_hIOCompletionPort;			// 完成端口的句柄
	HANDLE*				m_phWorkerThreads;				// 工作者线程的句柄指针数组
	int					m_nThreads;						// 生成的线程数量
	CRITICAL_SECTION	m_csMutex;						// 用于Worker线程同步的互斥量
	bool				m_bStart;						// 判断是否成功启动
};

#endif //__WIN__

#endif //_IOCP_INCLUDE_