#ifndef _NET_TCP_SERVER_
#define _NET_TCP_SERVER_

#include "nocopy.h"

class net_asyn_module;

class net_tcp_server : private nocopy
{
public:
	net_tcp_server(void);
	virtual ~net_tcp_server(void);
public:
	bool run();
protected:
	net_asyn_module* m_module;
};

#endif //_NET_TCP_SERVER_