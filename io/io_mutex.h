#ifndef _IO_MUTEX_H_
#define _IO_MUTEX_H_

#include "nocopy.h"
#include "io_base.h"
#ifdef __WIN__
#include <Windows.h>
#else
#include <pthread.h>
#endif

class io_mutex : private nocopy
{
	friend class io_lock;
	friend class io_trylock;
public:
	io_mutex();
	~io_mutex();
protected:
	void enter();
	bool try_enter();
	void exit();
private:
#ifdef __WIN__
	CRITICAL_SECTION m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
};

class io_lock : private nocopy
{
public:
	io_lock(io_mutex& m) : m_mutex(m)
	{
		m_mutex.enter();
	}
	~io_lock()
	{
		m_mutex.exit();
	}
private:
	io_mutex& m_mutex;
};

class io_trylock : private nocopy
{
public:
	io_trylock(io_mutex& m) : m_mutex(m)
	{
		m_is_enter = m_mutex.try_enter();
	}
	~io_trylock()
	{
		if (m_is_enter)
		{
			m_mutex.exit();
		}
	}
	bool try_again()
	{
		if (!m_is_enter)
		{
			m_is_enter = m_mutex.try_enter();
		}
		return m_is_enter;
	}
private:
	bool m_is_enter;
	io_mutex& m_mutex;
};


#endif //_IO_MUTEX_H_