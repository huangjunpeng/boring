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

	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int nRet = connect(socket, (sockaddr*)&addr, sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == nRet)
	{
		cout << "connect failed nret = " << nRet << std::endl;
		return SOCKET_ERROR;
	}



	char buff[1024] = {0};
	while (1) {
		memset(buff, 0, 1024);
		cin >> buff;	
		int nRet = ::send(socket, buff, strlen(buff), 0);
		if (SOCKET_ERROR == nRet)
		{
			cout << "send failed nret = " << nRet << std::endl;
			return SOCKET_ERROR;
		}
	}
	return 0;
}