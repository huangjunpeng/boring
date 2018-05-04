#include <iostream>
using namespace std;

#include <Address.h>
#include <inifile.h>

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
		return NULL;
	}

	cout << "www.baidu.com=" << CAddress::toHost("www.baidu.com") << endl;
	cout << "127.0.0.1=" << CAddress::toHost("127.0.0.1") << endl;

	CIniFile ini("D:\\Visual Studio 2008\\Projects\\vi\\tcp\\alex.ini");
	ini.init();

	ini.dump();

	CAddress a1("127.0.0.1", 8121);
	CAddress a2("192.168.1.240", 8121);
	CAddress a3("127.0.0.1", 8121);
	CAddress a4 = a2;

	if (a1 == a2) {
		cout << "a1==a2"<<endl;
	} else
	{
		cout << "a1!=a2"<<endl;
	}
	
	if (a1 == a3) {
		cout << "a1==a3"<<endl;
	} else
	{
		cout << "a1!=a3"<<endl;
	}
	cout << a1 << endl;
	cout << a2 << endl;
	cout << a3 << endl;
	cout << a4 << endl;
	getchar();

	return 0;
}