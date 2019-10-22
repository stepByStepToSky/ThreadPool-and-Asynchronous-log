#ifndef THREAD_H
#define THREAD_H

#pragma once

#include <pthread.h>
#include <functional>
#include <memory>

class Thread
{
public:
	typedef std::function<void ()> CallFunc;
	
	explicit Thread(CallFunc func) : m_bStarted(false), m_bJointed(false), m_pthreadId(0), m_func(func) {}
	~Thread()
	{
		if (m_bStarted && !m_bJointed)
		{
			pthread_detach(m_pthreadId);
		}
	}
	
	Thread(const Thread &) = delete;
	Thread & operator= (const Thread &) = delete;
	
	void Start();
	void Join();
	
private:
	bool m_bStarted;
	bool m_bJointed;
	pthread_t m_pthreadId;
	CallFunc m_func;
};

struct ThreadData
{
	typedef typename Thread::CallFunc CallFunc;
	
	ThreadData(CallFunc func) : m_func(func) {}
	
	CallFunc m_func;
};

void * ThreadRun(void * arg)
{
	ThreadData * ptData = static_cast<ThreadData *> (arg);
	ptData->m_func();
	delete ptData;
}

void Thread::Start()
{
	if (!m_bStarted)
	{
		ThreadData * ptData = new ThreadData(m_func);
		pthread_create(&m_pthreadId, NULL, ThreadRun, static_cast<void *> (ptData));
		m_bStarted = true;
	}
}

void Thread::Join()
{
	if (!m_bJointed)
	{
		pthread_join(m_pthreadId, NULL);
		m_bJointed = true;
	}
}

#endif