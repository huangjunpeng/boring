#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <boring.h>
#include <Address.h>
NAMESPACE_NET_BEGIN
class CInit_WSA
{
public:
	CInit_WSA();
	~CInit_WSA();
};

class CSocket_
{
public:
	CSocket_();
	~CSocket_();
private:
	uint32 m_fd;
};

NAMESPACE_NET_END
#endif