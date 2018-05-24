#include "Socket.h"
using namespace boring::net;

CInit_WSA g_initWsa;
static bool Startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return false;
	}

	if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2) {
		WSACleanup( );
		return false; 
	}

	return true;
}

static void Cleanup()
{
	WSACleanup();
}

CInit_WSA::CInit_WSA()
{
	std::cout << "init wsa ret = " << Startup() << std::endl;
}

CInit_WSA::~CInit_WSA()
{
	Cleanup();
	std::cout << "cleanup wsa ..."<< std::endl;
}

CSocket_::CSocket_()
{

}

CSocket_::~CSocket_()
{

}
