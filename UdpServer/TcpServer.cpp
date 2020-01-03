
#include "TcpServer.h"

CTcpServer::CTcpServer(void)
{
	m_pAddress = NULL;
	initSocket();
}

CTcpServer::CTcpServer(CAddress *pAddress)
{
	m_pAddress = pAddress;
	initSocket();
}

CTcpServer::~CTcpServer(void)
{
	if (m_pAddress)
	{
		delete m_pAddress;
		m_pAddress = NULL;
	}
}

int CTcpServer::Bind()
{
	return ::bind(
		m_socket, 
		(struct sockaddr*)&m_pAddress->GetSocketAddrIn(), 
		sizeof(struct sockaddr)
		);
}

int CTcpServer::Recv(SOCKET sock, char *buff, int len)
{
	int iLen = sizeof(struct sockaddr_in);
	int nRet = ::recv(sock, buff, len, 0);
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}

int CTcpServer::Send(SOCKET sock, char *buff, int len)
{
	int nRet = ::send(sock, buff, len, 0);
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}

SOCKET CTcpServer::Accept()
{
	sockaddr_in peerAddr = {0};
	int addrSize = sizeof(peerAddr);
	SOCKET sock = ::accept(m_socket, (sockaddr*)&peerAddr, &addrSize);

	return sock;
}

int CTcpServer::Listen(int backlog)
{
	int nRet = ::listen(m_socket, backlog);
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}
	return nRet;
}

void CTcpServer::initSocket()
{
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}
