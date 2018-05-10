#ifndef __ADDRESS_H_
#define __ADDRESS_H_
#include <boring.h>

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

class CAddr;
NAMESPACE_NET_BEGIN
typedef SharedPtr<CAddr> ShardCAddr;
class CAddress
{
public:
	CAddress(u_short uport);
	CAddress(const char *ip, u_short uport);
	CAddress(struct sockaddr_in addr);
	CAddress(CAddress& addr);
	CAddress& operator=(CAddress& addr);
	~CAddress();
	u_short toPort() const;
	std::string totIp() const;
	struct sockaddr_in GetSocketAddrIn();
	struct sockaddr_in GetSocketAddrIn() const;
	bool operator==(const CAddress& rAddr);
	friend std::ostream& operator<<(std::ostream& os, const CAddress& addr);
public:
	static int tcp_gethost(const char *addr, struct in_addr *inaddr);
	static std::string toHost(const char *addr);
private:
	ShardCAddr m_SharedPtrAddr;
};
NAMESPACE_NET_END
#endif //__ADDRESS_H_