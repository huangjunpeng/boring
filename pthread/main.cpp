#include <iostream>
#include <Windows.h>
#include <winsock.h>
using namespace std;

class nocopy
{
public:
	nocopy(){}
private:
	nocopy(const nocopy& cm);
	nocopy& operator=(const nocopy& cm);
};

class cx_mutex : public nocopy
{
public:
	cx_mutex()
	{
		m_icount = 0;
	}
private:
	int m_icount;
	friend class cx_lock;
};

class cx_lock : public nocopy
{
public:
	cx_lock(cx_mutex& cm) : m_cx_mutex(cm), m_lp_cx_mutex(new cx_mutex)
	{
	}
private:
	cx_mutex &m_cx_mutex;
	cx_mutex *m_lp_cx_mutex;
};

typedef int (*asyn_rw_cb)(int fd, void* data);
typedef int (*asyn_close_cb)(int fd);


class net_asyn_module : private nocopy
{
public:
	virtual int add(int fd, int event, asyn_rw_cb rcb, asyn_rw_cb wcb, asyn_close_cb ccb) = 0;
	virtual int del(int fd) = 0;
};

#ifdef WIN32
class net_wiocp_module : public net_asyn_module
{
public:
	virtual int add(int fd, int event, asyn_rw_cb rcb, asyn_rw_cb wcb, asyn_close_cb ccb)
	{
		return 0;
	}
	virtual int del(int fd) 
	{
		return 0;
	}
};
#else
class net_epoll_module : public net_asyn_module
{
public:
	virtual int add(int fd, int event, asyn_rw_cb rcb, asyn_rw_cb wcb, asyn_close_cb ccb)
	{
		return 0;
	}
	virtual int del(int fd) 
	{
		return 0;
	}
};
#endif

class net_address
{
public:
	net_address()
	{
		_addr_in.sin_family = AF_INET;
		_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
		_ip = inet_ntoa(_addr_in.sin_addr);
	}

	net_address(const char* ip, unsigned short port) : _ip(ip), _port(port)
	{
		if (NULL == ip) {
			_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
			_ip = inet_ntoa(_addr_in.sin_addr);
		} else {
			_addr_in.sin_addr.s_addr = inet_addr(_ip.c_str());
		}
		_addr_in.sin_family = AF_INET;
		_addr_in.sin_port = htons(port);
	}

	net_address(unsigned short port) : _port(port)
	{
		_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
		_addr_in.sin_family = AF_INET;
		_addr_in.sin_port = htons(port);
		_ip = inet_ntoa(_addr_in.sin_addr);
	}
public:
	const char* ip() 
	{ 
		return _ip.c_str(); 
	}

	void ip(const char* ip) 
	{ 
		if (NULL != ip) {
			_ip = ip;
			_addr_in.sin_addr.s_addr = inet_addr(_ip.c_str()); 
		}
		
	}

	unsigned short port() 
	{ 
		return _port; 
	}

	void port(unsigned short port) 
	{ 
		_port = port; 
		_addr_in.sin_port = htons(port);
	}

	operator struct sockaddr*() 
	{ 
		return (struct sockaddr*)&_addr_in;
	}
private:
	std::string _ip;
	unsigned short _port;
	struct sockaddr_in _addr_in;
};

class net_socket
{
public:
	net_socket(int type, int protocol) : _fd(INVALID_SOCKET)
	{
		_fd = ::socket(PF_INET, type, protocol);
	}

	net_socket(int fd) : _fd(fd)
	{
	}
public:
	bool is_create()
	{
		return (bool)(_fd != INVALID_SOCKET);
	}
protected:
	unsigned int _fd;
};



class net_tcp_socket : public net_socket
{
public:
	net_tcp_socket(net_asyn_module) : net_socket(SOCK_STREAM, IPPROTO_TCP)
	{
#ifdef WIN32
		_module = new net_wiocp_module;
#else
		_module = new net_epoll_module;
#endif
	}
public:
	virtual int send(const char* buf, size_t len){ return 0; }
	virtual int recv(char* buf, size_t len) { return 0; }
	virtual int asyn_send(const char* buf, size_t len) { return 0; }
	virtual int asyn_recv(char* buf, size_t len) { 	
		return 0; 
	}
private:
	net_asyn_module* _module;
};

class net_tcp_accepter : public net_socket
{
public:
	net_tcp_accepter(net_address addr) : net_socket(SOCK_STREAM, IPPROTO_TCP), _addr(addr)
	{
	}

	net_tcp_accepter(const char* ip, unsigned short port) : net_socket(SOCK_STREAM, IPPROTO_TCP), _addr(ip, port)
	{
	}
public:
	bool asyn_accept();
	bool accpet(net_tcp_socket& sock)
	{
		if (!listen(100)){
			return false;
		}

		sockaddr_in peerAddr = {0};
		int addrSize = sizeof(peerAddr);
		int fd = accept(_fd, (sockaddr*)&peerAddr, &addrSize);
		sock = net_tcp_socket(sock);
		return true;
	}

	bool bind(const char* ip, unsigned short port)
	{
		_addr = net_address(ip, port);
		return bind();
	}

	bool bind(const net_address& addr)
	{
		_addr = addr;
		return bind();
	}
	bool bind()
	{
		if (SOCKET_ERROR == ::bind(_fd, (struct sockaddr*)_addr, sizeof(struct sockaddr)))
		{
			return false;
		}
		return true;
	}

	bool listen(int backlog)
	{
		if (SOCKET_ERROR == ::listen(_fd, backlog))
		{
			return false;
		}
		return true;
	}
protected:
	net_address _addr;
	net_asyn_module* _module;
};

class net_udp_socket : public net_socket
{
public:
	net_udp_socket() : net_socket(SOCK_DGRAM, IPPROTO_UDP)
	{

	}
public:
	virtual int sendto(const char* buf, size_t len) = 0;
	virtual int recvfrom(char* buf, size_t len) = 0;
protected:
	net_address _addr;
};







class CThread
{
public:
	HANDLE m_hEvent;
};

HANDLE hEvent = NULL;



DWORD WINAPI thread1fun(LPVOID lpParameter)
{
	Sleep(12 * 1000);
	DWORD ret=WaitForSingleObject(hEvent,INFINITE);
	cout << "thread1fun" << endl;
	Sleep(60 * 1000);
	return 0;
}

DWORD WINAPI thread2fun(LPVOID lpParameter)
{
	Sleep(12 * 1000);
	DWORD ret=WaitForSingleObject(hEvent,INFINITE);
	cout << "thread2fun" << endl;
	Sleep(60 * 1000);
	return 0;
}



int main(int argc, char **argv)
{
	 cx_mutex cm;
	 cx_lock lock(cm);
	/*hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);


	CreateThread(NULL, 0, thread1fun, NULL, 0, NULL);
	CreateThread(NULL, 0, thread2fun, NULL, 0, NULL);
	Sleep(10 * 1000);

	SetEvent(hEvent);
	SetEvent(hEvent);

	Sleep(60 * 1000);*/

	hEvent = CreateSemaphore(
		NULL,
		0,
		10,
		NULL
		);
	
	LONG lpPreviousCount;

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	return 0;
}