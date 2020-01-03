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

template<class CallbackObj>
class CCallbackProxy
{
	typedef void(CallbackObj::*Call)(std::string* str);
public:
	void Set(CallbackObj *pObj, Call pCall);
	bool Exec(std::string *str);
private:
	CallbackObj *m_pObj;
	Call		 m_pCall;
};

template<class CallbackObj>
inline void CCallbackProxy<CallbackObj>::Set(CallbackObj *pObj, Call pCall)
{
	if (NULL == pObj || NULL == pCall)
	{
		throw std::string("�����������Ϊ��");
	}
	m_pCall = pCall;
	m_pObj	= pObj;
}
template<class CallbackObj>
inline bool CCallbackProxy<CallbackObj>::Exec(std::string *str)
{
	if (NULL == m_pObj || NULL == m_pCall)
	{
		return false;
	}
	(m_pObj->*m_pCall)(str);
	return true;
}


class Callback
{
public:
	Callback(std::string str) : m_str(str)
	{	
	}

	virtual void Printf(std::string* str)
	{
		cout << m_str.c_str() << endl;
	}

private:
	std::string m_str;
};

class Callback_Test : public Callback
{
public:
	Callback_Test(std::string str) : Callback(str)
	{

	}
	void Printf(std::string* str)
	{
		cout << str->c_str() << endl;
	}
};

typedef void (Callback::*callbcak)(std::string& str);
typedef void (Callback::*LPTELCONN)(std::string* str);

int main(int argc, char **argv)
{
	CCallbackProxy<Callback_Test> cProxy;
	

	Callback_Test bc("213");

	cProxy.Set(&bc, &Callback::Printf);

	std::string str = "123456789";
	cProxy.Exec(&str);
	


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