#include "Address.h"

class CAddr
{
public:
	friend class CAddress;
public:
	CAddr(struct sockaddr_in addr) : m_addr(addr)
	{
	}
	~CAddr()
	{
		std::cout << "~CAddr " << ::inet_ntoa(m_addr.sin_addr) << ":" << htons(m_addr.sin_port) << std::endl;
	}
private:
	struct sockaddr_in m_addr;
};

CAddress::CAddress(u_short uport)
{
	struct sockaddr_in addr = {0};
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(uport);
	m_SharedPtrAddr = ShardCAddr(new CAddr(addr));
}

CAddress::CAddress(const char *ip, u_short uport)
{
	struct sockaddr_in addr = {0};
	addr.sin_addr.s_addr = ::inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(uport);
	m_SharedPtrAddr = ShardCAddr(new CAddr(addr));
}

CAddress::CAddress(struct sockaddr_in addr)
{
	m_SharedPtrAddr = ShardCAddr(new CAddr(addr));
}

CAddress::CAddress(CAddress& addr)
{
	*this = addr;
}

CAddress& CAddress::operator=(CAddress& addr)
{
	m_SharedPtrAddr = ShardCAddr(new CAddr(addr.m_SharedPtrAddr->m_addr));
	return *this;
}

CAddress::~CAddress()
{
}

u_short CAddress::toPort() const
{
	u_short uport = ntohs(m_SharedPtrAddr->m_addr.sin_port);
	return uport;
}

std::string CAddress::totIp() const
{
	return ::inet_ntoa(m_SharedPtrAddr->m_addr.sin_addr);
}

struct sockaddr_in CAddress::GetSocketAddrIn()
{
	return m_SharedPtrAddr->m_addr;
}

struct sockaddr_in CAddress::GetSocketAddrIn() const
{
	return m_SharedPtrAddr->m_addr;
}

bool CAddress::operator==(const CAddress& rAddr)
{
	if (this->toPort() == rAddr.toPort() && this->totIp() == rAddr.totIp())
	{
		return true;
	}
	return false;
}

int CAddress::tcp_gethost(const char *addr, struct in_addr *inaddr)
{
	unsigned long iadd = -1;
	iadd = inet_addr((char*)addr);
	if (iadd != -1) {
		memcpy(inaddr, (const void*)&iadd, sizeof(iadd));
		return 0;
	}
	struct hostent *host = NULL;
	host = gethostbyname(addr);
	if (host) {
		memcpy(inaddr, host->h_addr, host->h_length);
		return 0;
	}	
	return -1;
}

std::string CAddress::toHost(const char *addr)
{
	unsigned long iadd = -1;
	iadd = inet_addr((char*)addr);
	if (iadd != -1) {
		return addr;
	}
	struct hostent *host = NULL;
	host = gethostbyname(addr);
	if (host) {
		struct in_addr inaddr;
		memcpy(&inaddr, host->h_addr, host->h_length);
		return inet_ntoa(inaddr);
	}	
	return addr;
}

std::ostream& operator<<(std::ostream& os, const CAddress& addr)
{
	os << addr.totIp() << ":" << addr.toPort();
	return os;
}
