#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include <Address.h>
using namespace boring::net;
class CUdpServer
{
public:
	CUdpServer();
	CUdpServer(CAddress *pAddress);
	~CUdpServer();

public:
	void SetAddress(CAddress *pAddress) { m_pAddress = pAddress;}
	int Bind();
	int RecvFrom(CAddress **pAddress, char *buff, int len);
	int SendTo(CAddress *pAddress, char *buff, int len);

	SOCKET GetSocket() { return m_socket; }

private:
	void initSocket();
private:
	CAddress *m_pAddress;
	SOCKET	m_socket;
};

#endif //_UDP_SERVER_H_