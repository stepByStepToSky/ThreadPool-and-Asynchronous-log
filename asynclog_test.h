#ifndef ASYNCLOG_H
#define ASYNCLOG_H

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <stdlib.h>
#include <unistd.h>
#include "mutex.h"
#include "condition.h"
#include "thread.h"
#include "fixedbuffer.h"
#include "logfile.h"


//Mutex m_mutex;
//Condition m_subThreadFlushCond(m_mutex);



class AsyncLog
{
public:
	
	AsyncLog();
	~AsyncLog()
	{
		if(m_bRunning)
		{
			Stop();
		}
	}
	AsyncLog(const AsyncLog &) = delete;
	AsyncLog & operator= (const AsyncLog &) = delete;
	
	void Start()
	{
		m_bRunning = true;
		m_thread.Start();
		//LockGuard lock(m_mutex);
		//m_mainThreadRunCond.Wait();
		//m_mainThreadRunCond.WaitForSeconds(3);
		std::cout << "Main thread Running " << __LINE__ << std::endl;
	}
	
	void Stop()
	{
		m_bRunning = false;
		LockGuard lock(this->m_mutex);
		this->m_subThreadFlushCond.Notify();
		m_thread.Join();
	}
	
	void ThreadFunc();
	void NotifyLog()
	{
		LockGuard lock(this->m_mutex);
		this->m_subThreadFlushCond.Notify();
	}
private:
	bool m_bRunning;
	Thread m_thread;
	//std::unique_ptr<Mutex> m_mutex;
	//std::unique_ptr<Condition> m_subThreadFlushCond;
	Mutex m_mutex;
	Condition m_subThreadFlushCond;
};

AsyncLog::AsyncLog()
	: m_bRunning(false),
	m_thread(std::bind(&AsyncLog::ThreadFunc, this)),
	//m_mutex(new Mutex),
	//m_subThreadFlushCond(new Condition(*m_mutex))
	m_mutex(),
	m_subThreadFlushCond(m_mutex)
{
}

void AsyncLog::ThreadFunc()
{
	std::cout << "Sub thread, " << __LINE__ << " Running " << std::endl;
	while(m_bRunning)
	{
		{
			LockGuard lock(this->m_mutex);
			if (true)
			{
				std::cout << "Sub thread, wait for Condition, " << __LINE__ << std::endl;
				this->m_subThreadFlushCond.WaitForSeconds(1);
				//m_subThreadFlushCond.Wait();
			}
			std::cout << "Sub thread, " << __LINE__ << std::endl;
		}
	}
}




#endif