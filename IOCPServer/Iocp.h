#ifndef _IOCP_INCLUDE_
#define _IOCP_INCLUDE_

//��ʱ
#define EV_TIMEOUT	0x01
//�ɶ�
#define EV_READ		0x02
//��д
#define EV_WRITE	0x04
//����
#define EV_ACCEPT	0x08

#ifdef WIN32
#include "config_win32.h"
#endif

#ifdef __WIN__
#include <winsock2.h>

struct IocpOverlapped_;


typedef void (*IcopCallback)(IocpOverlapped_ *, SOCKET, DWORD, bool success);

typedef struct IocpOverlapped_ {
	OVERLAPPED overlapped;
	IcopCallback cb;
	void* data;
}IocpOverlapped, *LPIocpOverlapped;




class CIocp
{
public:
	CIocp(int nThreads);
	~CIocp();

	//��������
	bool Start();

	// ֹͣ����
	void Stop();

	// ������󶨵���ɶ˿���
	bool _AssociateWithIOCP(SOCKET fd, uintptr_t key);

protected:
	// ��ʼ��IOCP
	bool _InitializeIOCP();

	// ����ͷ���Դ
	void _DeInitialize();

	// ��̬ �̺߳�����ΪIOCP�������Ĺ������߳�
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);
private:
	HANDLE				m_hShutdownEvent;				// ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�
	HANDLE				m_hIOCompletionPort;			// ��ɶ˿ڵľ��
	HANDLE*				m_phWorkerThreads;				// �������̵߳ľ��ָ������
	int					m_nThreads;						// ���ɵ��߳�����
	CRITICAL_SECTION	m_csMutex;						// ����Worker�߳�ͬ���Ļ�����
	bool				m_bStart;						// �ж��Ƿ�ɹ�����
};

#endif //__WIN__

#endif //_IOCP_INCLUDE_