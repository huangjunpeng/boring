#include <iostream>
using namespace std;

#include <Address.h>
#include <inifile.h>
#include <ProtocolHeader.h>
#include <execption.h>
#include "TcpServer.h"
using namespace boring::base;
using namespace boring::net;

int main(int argc, char **argv)
{

	WSADATA wsaData;
	int iResult;
	// 此处应添加的程序如下
	// 1.  当初始化winsocket
	iResult = WSAStartup(MAKEWORD(2,2),&wsaData);/* & 取地址*/
	// 2.  检查该socket是否初始化成功，即该socket是否等于0；如果初始化不成功，应当给出错误报警，并结束程序。
	if(iResult!=0)
	{
		printf("初始化失败!\n");
		return 0;
	}

	CTcpServer *pServer = new CTcpServer(new CAddress(12345));
	if (SOCKET_ERROR == pServer->Bind()) {
		printf("绑定失败, nRet=%d\n", WSAGetLastError());
		return 0;
	}

	if (SOCKET_ERROR == pServer->Listen(20)) {
		printf("监听失败, nRet=%d\n", WSAGetLastError());
		return 0;
	}

	char buf[1024] = {0};
	int len = 0;
	while (1) {
		SOCKET sock = pServer->Accept();
		memset(buf, 0, 1024);
		len = 0;
		if (SOCKET_ERROR == (len = pServer->Recv(sock, buf, 1023))) {
			printf("接收客户端数据失败, nRet=%d\n", WSAGetLastError());
			continue;
		}
		cout << "客户端: " << sock << " Say:" << buf << endl;
	}
	return 0;
}