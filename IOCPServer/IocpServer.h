#ifndef _IOCP_SERVER_H_
#define _IOCP_SERVER_H_

#include <boring.h>
#include <Address.h>
#include <Socket.h>

using namespace boring::net;
typedef SharedPtr<CAddress> ShardCAddress;


class CIocpServer
{
public:
	CIocpServer();
	CIocpServer(ShardCAddress Addr);
	~CIocpServer();
private:
	CSocket_ m_sock;
};
#endif