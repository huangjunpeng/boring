#include <iostream>
using namespace std;

#include <Address.h>
#include <inifile.h>
#include <ProtocolHeader.h>
#include <execption.h>
#include "UdpServer.h"
using namespace boring::base;
using namespace boring::net;

int main(int argc, char **argv)
{

	WSADATA wsaData;
	int iResult;
	// �˴�Ӧ��ӵĳ�������
	// 1.  ����ʼ��winsocket
	iResult = WSAStartup(MAKEWORD(2,2),&wsaData);/* & ȡ��ַ*/
	// 2.  ����socket�Ƿ��ʼ���ɹ�������socket�Ƿ����0�������ʼ�����ɹ���Ӧ���������󱨾�������������
	if(iResult!=0)
	{
		printf("��ʼ��ʧ��!\n");
		return 0;
	}

	CUdpServer *pServer = new CUdpServer(new CAddress(5213));
	if (SOCKET_ERROR == pServer->Bind()) {
		printf("��ʧ��, nRet=%d\n", WSAGetLastError());
	}
	CAddress pAddress;
	char buf[1024] = {0};
	int len = 0;
	while (1) {
		memset(buf, 0, 1024);
		len = 0;
		if (SOCKET_ERROR == (len = pServer->RecvFrom(pAddress, buf, 1023))) {
			printf("���տͻ�������ʧ��, nRet=%d\n", WSAGetLastError());
			continue;
		}
		cout << "�ͻ���:" << pAddress << " Say:" << buf << endl;
		
		if (SOCKET_ERROR == (len = pServer->SendTo(pAddress, buf, len)))
		{
			return SOCKET_ERROR;
		}
	}
	return 0;
}