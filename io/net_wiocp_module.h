#ifndef _NET_WIOCP_MODULE_H_
#define _NET_WIOCP_MODULE_H_


#include <winsock2.h>
#include <mswsock.h>



#include "io_base.h"
#include "net_asyn_module.h"
#include "io_mutex.h"

#ifdef __WIN__

struct net_win32_overlapped {
	OVERLAPPED overlapped;
	iocp_callback cb;
};

struct win32_extension_fns {
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSocketAddr;
};

class net_wiocp_module : public net_asyn_module
{
public:
	net_wiocp_module();
	virtual ~net_wiocp_module();
public:
	bool start();
	bool associate(io_uint32 fd, io_uintptr_t key);
	void shutdown();
public:
	static const struct win32_extension_fns *event_get_win32_extension_fns();
protected:
	static DWORD WINAPI loop(LPVOID lpParam);
	void notify_all();
private:
	HANDLE		m_handle_iocp;
	int			m_ithreads;
	HANDLE*		m_phandle_threads;
	HANDLE		m_handle_shutdown;
};

#endif //__WIN__

#endif //_NET_WIOCP_MODULE_H_