#pragma once

#include <Address.h>
using namespace boring::net;

class CTcpServer
{
public:
	CTcpServer(void);
	CTcpServer(CAddress *pAddress);
	~CTcpServer(void);
public:
	void SetAddress(CAddress *pAddress) { m_pAddress = pAddress;}
	int Bind();
	int Recv(SOCKET sock, char *buff, int len);
	int Send(SOCKET sock, char *buff, int len);
	SOCKET Accept();
	int Listen(int backlog);

	SOCKET GetSocket() { return m_socket; }

private:
	void initSocket();
private:
	CAddress *m_pAddress;
	SOCKET	m_socket;
};