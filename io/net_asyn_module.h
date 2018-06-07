#ifndef _NET_ASYN_MODULE_H_
#define _NET_ASYN_MODULE_H_

#include "nocopy.h"
#include "io_base.h"


typedef void (*iocp_callback)(void *, io_uint32, io_size_t, int success);

class net_asyn_module : private nocopy
{
public:
	net_asyn_module();
	virtual ~net_asyn_module();
public:
	virtual bool associate(io_uint32 fd, io_uintptr_t key) = 0;
};

#endif //_NET_ASYN_MODULE_H_