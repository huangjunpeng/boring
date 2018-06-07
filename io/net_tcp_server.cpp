#include "net_tcp_server.h"
#include "net_wiocp_module.h"

net_tcp_server::net_tcp_server(void) : m_module(new net_wiocp_module)
{
}

net_tcp_server::~net_tcp_server(void)
{
}
