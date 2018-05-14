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

int CUdpServer::RecvFrom(CAddress &pAddress, char *buff, int len)
{
	int iLen = sizeof(struct sockaddr_in);
	int nRet = ::recvfrom(m_socket, buff, len, 0, (struct sockaddr*)pAddress, &iLen);
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}

int CUdpServer::SendTo(const CAddress &pAddress, char *buff, int len)
{
	int nRet = ::sendto(m_socket, buff, len, 0, (struct sockaddr*)pAddress, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}
