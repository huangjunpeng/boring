#include "net_wiocp_module.h"

#ifdef __WIN__

// 释放句柄宏
#define RELEASE_HANDLE(x)  {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}

// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL

#pragma comment(lib, "ws2_32.lib")

static int 
get_cpus()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

static void* 
get_exten_sion_fun(io_uint32 fd, const GUID* guid)
{
	void *ptr = NULL;
	DWORD bytes=0;
	WSAIoctl(fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
		(GUID*)guid, sizeof(*guid),
		&ptr, sizeof(ptr),
		&bytes, NULL, NULL);
	return ptr;
}

static int extension_fns_initialized = 0;
static void
init_extension_functions(struct win32_extension_fns *ext)
{
	const GUID acceptex = WSAID_ACCEPTEX;
	const GUID getacceptexsockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
		return;
	ext->lpfnAcceptEx = (LPFN_ACCEPTEX)get_exten_sion_fun(s, &acceptex);
	ext->lpfnGetAcceptExSocketAddr = (LPFN_GETACCEPTEXSOCKADDRS)get_exten_sion_fun(s,&getacceptexsockaddrs);
	closesocket(s);
	extension_fns_initialized = 1;
}

static struct win32_extension_fns the_extension_fns;
const struct win32_extension_fns * net_wiocp_module::event_get_win32_extension_fns()
{
	return &the_extension_fns;
}

net_wiocp_module::net_wiocp_module() : m_phandle_threads(NULL),
					m_handle_iocp(NULL),
					m_handle_shutdown(NULL),
					m_ithreads(0)
{
	m_ithreads = get_cpus() * 2 + 2;
	if (extension_fns_initialized == 0)
	{
		init_extension_functions(&the_extension_fns);
	}
}

net_wiocp_module::~net_wiocp_module()
{
	RELEASE_HANDLE(m_handle_iocp);

	for (int i = 0; i < m_ithreads; i++)
	{
		RELEASE_HANDLE(m_phandle_threads[i]);
	}

	RELEASE_HANDLE(m_handle_shutdown);

	_delete_a(m_phandle_threads);
}

bool net_wiocp_module::start()
{
	//初始化系统退出事件
	m_handle_shutdown = ::CreateEvent(
		NULL, 
		TRUE, 
		FALSE, 
		NULL
	);

	m_handle_iocp = ::CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		0
	);
	
	m_phandle_threads = new HANDLE[m_ithreads];

	DWORD nid;
	for (int i = 0; i < m_ithreads; i++)
	{
		m_phandle_threads[i] = ::CreateThread(
			0,
			0,
			loop,
			this,
			0,
			&nid
		);
	}

	return true;
}

DWORD WINAPI net_wiocp_module::loop(LPVOID lpParam)
{
	net_wiocp_module* wiocp = (net_wiocp_module*)lpParam;
	HANDLE p = wiocp->m_handle_iocp;
	OVERLAPPED *overlapped = NULL;
	ULONG_PTR key = 0;
	DWORD bytes = 0;
	while (WAIT_OBJECT_0 != WaitForSingleObject(wiocp->m_handle_shutdown, 0))
	{
		int ok = GetQueuedCompletionStatus(p, &bytes, (PULONG_PTR)&key,
					&overlapped,
					INFINITE
				);
		if (key != EXIT_CODE && overlapped)
		{
			net_win32_overlapped* eo = CONTAINING_RECORD(overlapped, net_win32_overlapped, overlapped);
			eo->cb(eo, key, bytes, ok);
		}
		else if (!overlapped)
		{
			break;
		}
	}
	return 0;
}

void net_wiocp_module::shutdown()
{
	SetEvent(m_handle_shutdown);

	this->notify_all();
}

void net_wiocp_module::notify_all()
{
	for (int i = 0; i < m_ithreads; i++)
	{
		//通知所有完成端口操作退出
		PostQueuedCompletionStatus(
			m_handle_iocp,
			0,
			(DWORD)EXIT_CODE,
			NULL
		);
	}

	WaitForMultipleObjects(m_ithreads, m_phandle_threads, TRUE, INFINITE);
}

bool net_wiocp_module::associate(io_uint32 fd, io_uintptr_t key)
{
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)fd, m_handle_iocp, (DWORD)key, m_ithreads);
	if (NULL == hTemp)
	{
		return false;
	}

	return true;
}
#endif //__WIN__