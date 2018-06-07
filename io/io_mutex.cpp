#include "io_mutex.h"

io_mutex::io_mutex()
{
#ifdef __WIN__
	InitializeCriticalSection(&m_mutex);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
}

io_mutex::~io_mutex()
{
#ifdef __WIN__
	DeleteCriticalSection(&m_mutex);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}

void io_mutex::enter()
{
#ifdef __WIN__
	EnterCriticalSection(&m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}

bool io_mutex::try_enter()
{
#ifdef __WIN__
	return TryEnterCriticalSection(&m_mutex);
#else
	return (pthread_mutex_trylock(&m_mutex) == 0);
#endif
}

void io_mutex::exit()
{
#ifdef __WIN__
	LeaveCriticalSection(&m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}
