#include <iostream>
#include <Windows.h>
using namespace std;
class CThread
{
public:
	HANDLE m_hEvent;
};

HANDLE hEvent = NULL;



DWORD WINAPI thread1fun(LPVOID lpParameter)
{
	Sleep(12 * 1000);
	DWORD ret=WaitForSingleObject(hEvent,INFINITE);
	cout << "thread1fun" << endl;
	Sleep(60 * 1000);
	return 0;
}

DWORD WINAPI thread2fun(LPVOID lpParameter)
{
	Sleep(12 * 1000);
	DWORD ret=WaitForSingleObject(hEvent,INFINITE);
	cout << "thread2fun" << endl;
	Sleep(60 * 1000);
	return 0;
}



int main(int argc, char **argv)
{
	/*hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);


	CreateThread(NULL, 0, thread1fun, NULL, 0, NULL);
	CreateThread(NULL, 0, thread2fun, NULL, 0, NULL);
	Sleep(10 * 1000);

	SetEvent(hEvent);
	SetEvent(hEvent);

	Sleep(60 * 1000);*/

	hEvent = CreateSemaphore(
		NULL,
		0,
		10,
		NULL
		);
	
	LONG lpPreviousCount;

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	ReleaseSemaphore(
			hEvent,
			1,
			&lpPreviousCount
		);

	return 0;
}