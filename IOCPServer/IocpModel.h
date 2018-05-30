#ifndef _IOCP_MODEL_H_
#define _IOCP_MODEL_H_

#ifdef WIN32
#include "config_win32.h"
#endif

#include <vector>
#include <algorithm>

#ifdef __WIN__

#include <winsock2.h>
#include <MSWSock.h>


// ���������� (1024 * 8)
#define MAX_BUFFER_LEN        8192 

// ����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum _OPERATION_TYPE
{
	POSTED_ACCEPT,				// ��־Ͷ�ݵ�Accept����
	POSTED_SEND,				// ��־Ͷ�ݵ��Ƿ��Ͳ���
	POSTED_RECV,				// ��־Ͷ�ݵ��ǽ��ղ���
	POSTED_NULL					// ���ڳ�ʼ����������
}OPERATION_TYPE;


//��IO���ݽṹ�嶨��(����ÿһ���ص������Ĳ���)
typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED		m_Overlapped;					// ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)
	SOCKET			m_sockAccept;					// ������������ʹ�õ�Socket
	WSABUF			m_wsaBuf;						// WSA���͵Ļ����������ڸ��ص�������������
	char			m_szBuffer[MAX_BUFFER_LEN];		// �����WSABUF�������ַ��Ļ�����
	OPERATION_TYPE	m_OpType;						// ��ʶ�������������(��Ӧ�����ö��)
	bool			m_busy;							//��ʶ��IO�����Ƿ���æµ�У�trueΪæ���С���Ҫ���ڷ������ݵ��ж�

	// ��ʼ��
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType     = POSTED_NULL;
		m_busy       = false;
	}

	// �ͷŵ�Socket
	~_PER_IO_CONTEXT()
	{
		if(m_sockAccept != INVALID_SOCKET)
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}

	// ���û���������
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}
}PER_IO_CONTEXT, *PPER_IO_CONTEXT;


//��������ݽṹ�嶨��(����ÿһ����ɶ˿ڣ�Ҳ����ÿһ��Socket�Ĳ���)
typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET							m_Socket;			// ÿһ���ͻ������ӵ�Socket
	SOCKADDR_IN						m_ClientAddr;		// �ͻ��˵ĵ�ַ
	std::string						m_strIP;			// �ͻ��˵�IP��ַ
	unsigned short					m_usPort;			// �ͻ��˵Ķ˿ں�
	std::vector<_PER_IO_CONTEXT*>	m_arrayIoContext;	// �ͻ���������������������ݣ�
														// Ҳ����˵����ÿһ���ͻ���Socket���ǿ���������ͬʱͶ�ݶ��IO�����
	// ��ʼ��
	_PER_SOCKET_CONTEXT()
	{
		m_Socket	= INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
		m_usPort	= 0;
	}

	// �ͷ���Դ
	~_PER_SOCKET_CONTEXT()
	{
		if (m_Socket != INVALID_SOCKET) 
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}

		for (std::vector<_PER_IO_CONTEXT*>::iterator it = m_arrayIoContext.begin(); it != m_arrayIoContext.end(); ++it)
		{
			_PER_IO_CONTEXT* pIoContext = *it;
			if (pIoContext) {
				delete pIoContext;
				pIoContext = NULL;
			}
		}
		m_arrayIoContext.clear();
	}

	// ��ȡһ���µ�pIoContext
	_PER_IO_CONTEXT* GetNewIoContext()
	{
		_PER_IO_CONTEXT* pIoContext = new _PER_IO_CONTEXT;
		m_arrayIoContext.push_back(pIoContext);
		return pIoContext;
	}

	// ���������Ƴ�һ��ָ����pIoContext
	void RemoveContext(_PER_IO_CONTEXT* pIoContext)
	{
		if (NULL == pIoContext) 
		{
			return;
		}
		std::vector<_PER_IO_CONTEXT*>::iterator it = std::find(m_arrayIoContext.begin(), m_arrayIoContext.end(), pIoContext);
		if (it != m_arrayIoContext.end())
		{
			delete pIoContext;
			pIoContext = NULL;
			m_arrayIoContext.erase(it);
		}
	}

}PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;


//�����̶߳��Ǿ�̬�������̺߳����Ǿ�̬������
typedef struct _THREADPARAMS_WORKER
{
	void*		pIOCPModel;                                   // ��ָ�룬���ڵ������еĺ���
	int         nThreadNo;                                    // �̱߳��

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER; 

typedef void (*log)(const char* format, ...);

class CIocpModel
{
public:
	CIocpModel(const char* ip, unsigned short port);
	~CIocpModel();
public:
	
	//��������
	bool Start();

	// ֹͣ����
	void Stop();

protected:

	// ��ʼ��IOCP
	bool _InitializeIOCP();

	// ��ʼ��Socket
	bool _InitializeListenSocket();

	// ����ͷ���Դ
	void _DeInitialize();

	// ��ñ����Ĵ���������
	int _GetNoOfProcessors();

	// ��̬ �̺߳�����ΪIOCP�������Ĺ������߳�
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);
private:
	HANDLE								m_hShutdownEvent;				// ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�

	HANDLE								m_hIOCompletionPort;			// ��ɶ˿ڵľ��

	HANDLE*								m_phWorkerThreads;				// �������̵߳ľ��ָ������

	int									m_nThreads;						// ���ɵ��߳�����

	std::string							m_strIP;						// �������˵�IP��ַ

	unsigned short						m_usPort;						// �������˵ļ����˿�

	CRITICAL_SECTION					m_csMutex;						// ����Worker�߳�ͬ���Ļ�����

	bool								m_bStart;						

	LPFN_ACCEPTEX						m_lpfnAcceptEx;					// AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
	
	LPFN_GETACCEPTEXSOCKADDRS			m_lpfnGetAcceptExSocketAddr;

	std::vector<PPER_SOCKET_CONTEXT>	m_arrayClientContext;			// �ͻ���Socket��Context��Ϣ

	PPER_SOCKET_CONTEXT					m_pListenContext;				// ���ڼ�����Socket��Context��Ϣ

	log									m_log_cb;
};

#endif

#endif