#include "UdpServer.h"
#include <boring.h>
#include <execption.h>

CUdpServer::CUdpServer() : m_pAddress(NULL), m_socket(INVALID_SOCKET)
{
	initSocket();
}

CUdpServer::CUdpServer(CAddress *pAddress) : m_pAddress(pAddress), m_socket(INVALID_SOCKET)
{
	initSocket();
}

CUdpServer::~CUdpServer(void)
{
	_delete(m_pAddress);
}

void CUdpServer::initSocket()
{
	m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int CUdpServer::Bind()
{
	return ::bind(
		m_socket, 
		(struct sockaddr*)&m_pAddress->GetSocketAddrIn(), 
		sizeof(struct sockaddr)
		);
}

int CUdpServer::RecvFrom(CAddress **pAddress, char *buff, int len)
{
	struct sockaddr_in addr;
	int iLen = sizeof(addr);
	int nRet = ::recvfrom(m_socket, buff, len, 0, (struct sockaddr*)&addr, &iLen);
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	*pAddress = new CAddress(addr);
	return nRet;
}

int CUdpServer::SendTo(CAddress *pAddress, char *buff, int len)
{
	struct sockaddr_in addr = pAddress->GetSocketAddrIn();
	int nRet = ::sendto(m_socket, buff, len, 0, (struct sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}
