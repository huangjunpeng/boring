#include <iostream>
#include "IocpServer.h"
#include "IocpModel.h"

using namespace std;

CIocpModel* pIocpModel = NULL;


#ifdef WIN32
/**********************************************************************************************
	CTRL_C_EVENT			- ���û�������CTRL+C,������GenerateConsoleCtrlEvent API����. 
	CTRL_BREAK_EVENT		- �û�����CTRL+BREAK, ������GenerateConsoleCtrlEvent API����.
	CTRL_CLOSE_EVENT		- ����ͼ�رտ���̨����ϵͳ���͹ر���Ϣ��
	CTRL_LOGOFF_EVENT		- �û��˳�ʱ�����ǲ��ܾ������ĸ��û�. 
	CTRL_SHUTDOWN_EVENT		- ��ϵͳ���ر�ʱ.
************************************************************************************************/
BOOL WINAPI Handler(DWORD dwEvent)
{
	switch(dwEvent)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_C_EVENT:
		{
			/* shutdown the server */
			if (pIocpModel)
			{
				pIocpModel->Stop();
			}
		}
		return TRUE;
	}
	return FALSE;
}
#else
/****************************************************************************************************************************************
	SIGINT		������ֹ(interrupt)�ź�, ���û�����INTR�ַ�(ͨ����Ctrl-C)ʱ����������֪ͨǰ̨��������ֹ����
	SIGQUIT		��SIGINT����, ����QUIT�ַ�(ͨ����Ctrl-\)������. ���������յ�SIGQUIT�˳�ʱ�����core�ļ�, �����������������һ����������ź�
	SIGTERM		�������(terminate)�ź�, ��SIGKILL��ͬ���Ǹ��źſ��Ա������ʹ���ͨ������Ҫ������Լ������˳���shell����killȱʡ��������źš����������ֹ���ˣ����ǲŻ᳢��SIGKILL
	SIGSTOP		ֹͣ(stopped)���̵�ִ��. ע������terminate�Լ�interrupt������:�ý��̻�δ����, ֻ����ִͣ��. ���źŲ��ܱ�����, ��������
********************************************************************************************************************************************************************/
void Handler(int event)
{
	switch (event)
	{
	case SIGINT:
	case SIGSTOP:
		{
			if (pIocpModel)
			{
				pIocpModel->Stop();
			}
		}
		break;
	}
}
#endif


int main(int argc, char **argv)
{
#ifdef WIN32
	if(!SetConsoleCtrlHandler(Handler, TRUE))
	{
		fprintf(stderr, "error setting event handler.\n" );
		return -1;
	}
#else
	signal(SIGINT, Handler);
#endif

	cout << "Hello World!" << endl;

	pIocpModel = new CIocpModel("127.0.0.1", 8888);
	pIocpModel->Start();
	getchar();

	return 0;
}