#include <iostream>
#include "IocpServer.h"
#include "IocpModel.h"

using namespace std;

CIocpModel* pIocpModel = NULL;


#ifdef WIN32
/**********************************************************************************************
	CTRL_C_EVENT			- 当用户按下了CTRL+C,或者由GenerateConsoleCtrlEvent API发出. 
	CTRL_BREAK_EVENT		- 用户按下CTRL+BREAK, 或者由GenerateConsoleCtrlEvent API发出.
	CTRL_CLOSE_EVENT		- 当试图关闭控制台程序，系统发送关闭消息。
	CTRL_LOGOFF_EVENT		- 用户退出时，但是不能决定是哪个用户. 
	CTRL_SHUTDOWN_EVENT		- 当系统被关闭时.
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
	SIGINT		程序终止(interrupt)信号, 在用户键入INTR字符(通常是Ctrl-C)时发出，用于通知前台进程组终止进程
	SIGQUIT		和SIGINT类似, 但由QUIT字符(通常是Ctrl-\)来控制. 进程在因收到SIGQUIT退出时会产生core文件, 在这个意义上类似于一个程序错误信号
	SIGTERM		程序结束(terminate)信号, 与SIGKILL不同的是该信号可以被阻塞和处理。通常用来要求程序自己正常退出，shell命令kill缺省产生这个信号。如果进程终止不了，我们才会尝试SIGKILL
	SIGSTOP		停止(stopped)进程的执行. 注意它和terminate以及interrupt的区别:该进程还未结束, 只是暂停执行. 本信号不能被阻塞, 处理或忽略
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
		throw std::string("传入参数不能为空");
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