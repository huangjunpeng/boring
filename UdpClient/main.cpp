#include <iostream>
using namespace std;


#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

class CServer
{
public:
	SOCKET m_socket;
	struct sockaddr_in addr;
};

unsigned long __stdcall ThreadPro(void *lpThreadParameter)
{
	CServer *pServer = (CServer*)lpThreadParameter;
	int nRet = 0;
	char buff[1024] = {0};
	while (1) {
		memset(buff, 0, 1024);
		cin >> buff;		
		nRet = ::sendto(pServer->m_socket, buff, strlen(buff), 0, (struct sockaddr*)&pServer->addr, sizeof(pServer->addr));
		if (SOCKET_ERROR == nRet)
		{
			return SOCKET_ERROR;
		}
	}
	return 0;
}


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

	SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5213);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	char buff[1024] = {0};
	cin >> buff;		
	int nRet = ::sendto(socket, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == nRet)
	{
		return SOCKET_ERROR;
	}

	CServer *pServer = new CServer;
	pServer->addr = addr;
	pServer->m_socket = socket;

	::CreateThread(NULL, NULL, ThreadPro, (void*)pServer, THREAD_PRIORITY_NORMAL, NULL);

	char buf[1024] = {0};
	int iLen = 0;
	while (1) {
		memset(buf, 0, 1024);
		iLen = sizeof(addr);
		nRet = ::recvfrom(socket, buf, 1023, 0, (struct sockaddr*)&addr, &iLen);
		if (SOCKET_ERROR == nRet)
		{
			return SOCKET_ERROR;
		}
		cout << "������:127.0.0.1:5213 Say:" << buf << endl;
	}
	return 0;
}