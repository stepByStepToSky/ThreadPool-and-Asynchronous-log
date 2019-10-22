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

static const size_t kFixedBufferSize = 4096;

class AsyncLog
{
public:
	typedef FixedBuffer<kFixedBufferSize> Buffer;
	typedef std::unique_ptr<Buffer> BufferPtr;
	
	AsyncLog();
	~AsyncLog()
	{
		if(m_bRunning)
		{
			Stop();
		}
		Sync();
	}
	AsyncLog(const AsyncLog &) = delete;
	AsyncLog & operator= (const AsyncLog &) = delete;
	
	void Start()
	{
		m_bRunning = true;
		m_thread.Start();
		LockGuard lock(*m_mutex);
		m_mainThreadRunCond->Wait();
		std::cout << "Main thread Running " << __LINE__ << std::endl;
	}
	
	void Stop()
	{
		m_bRunning = false;
		m_subThreadFlushCond->Notify();
		m_thread.Join();
	}
	
	void Sync()
	{
		m_logFilePtr->Sync();
	}
	
	void Append(const char * data, size_t writeSize);
	void ThreadFunc();
	void NotifyLog()
	{
		m_subThreadFlushCond->Notify();
	}
private:
	bool m_bRunning;
	std::unique_ptr<Mutex> m_mutex;
	std::unique_ptr<Condition> m_mainThreadRunCond;
	std::unique_ptr<Condition> m_subThreadFlushCond;
	Thread m_thread;
	BufferPtr m_curBuff;
	BufferPtr m_nextBuff;
	std::vector<BufferPtr> m_buffers;
	std::unique_ptr<LogFile> m_logFilePtr;
};

AsyncLog::AsyncLog()
	: m_bRunning(false),
	m_mutex (new Mutex), 
	m_mainThreadRunCond(new Condition(*m_mutex)),
	m_subThreadFlushCond(new Condition(*m_mutex)), 
	m_thread(std::bind(&AsyncLog::ThreadFunc, this)), 
	m_curBuff(new Buffer), 
	m_nextBuff(new Buffer),
	m_logFilePtr(new LogFile)
{
	m_buffers.reserve(8);
}

void AsyncLog::Append(const char * data, size_t writeSize)
{
	//std::cout << "Main thread, " << __LINE__ << std::endl;
	LockGuard lock(*m_mutex);
	if (m_curBuff->Avail() > writeSize)
	{
		//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " m_curBuff->Avail()="  << m_curBuff->Avail() << " writeSize=" << writeSize << std::endl;
		m_curBuff->Append(data, writeSize);
	}
	else
	{
		m_buffers.push_back(std::move(m_curBuff));
		if (m_nextBuff)
		{
			m_curBuff = std::move(m_nextBuff);
		}
		else
		{
			m_curBuff.reset(new Buffer);
		}
		m_curBuff->Append(data, writeSize);
		m_subThreadFlushCond->Notify();
	}
	//std::cout << "Main thread, " << __LINE__ << std::endl;
}

void AsyncLog::ThreadFunc()
{
	BufferPtr firstBuffer(new Buffer);
	BufferPtr secondBuffer(new Buffer);
	std::vector<BufferPtr> vecWriteBuffers;
	vecWriteBuffers.reserve(16);
	{
		LockGuard lock(*m_mutex);
		m_mainThreadRunCond->Notify();
	}
	std::cout << "Sub thread, " << __LINE__ << " Running " << std::endl;
	while(m_bRunning)
	{
		{
			LockGuard lock(*m_mutex);
			if (m_buffers.empty())
			{
				std::cout << "Sub thread, wait for Condition, " << __LINE__ << std::endl;
				m_subThreadFlushCond->WaitForSeconds(3);
				//m_subThreadFlushCond.Wait();
			}
			std::cout << "Sub thread, " << __LINE__ << " Buffer size, " <<m_buffers.size() << std::endl;
			
			m_buffers.push_back(std::move(m_curBuff));
			m_curBuff = std::move(firstBuffer);
			m_curBuff->Reset();
			if (!m_nextBuff)
			{
				m_nextBuff = std::move(secondBuffer);
				m_nextBuff->Reset();
			}
			
			m_buffers.swap(vecWriteBuffers);
			m_buffers.clear();
		}
		
		for (size_t i = 0; i < vecWriteBuffers.size(); ++i)
		{
			m_logFilePtr->Append(vecWriteBuffers[i]->Data(), vecWriteBuffers[i]->Size());
		}
		
		firstBuffer = std::move(vecWriteBuffers[0]);
		if (2 <= vecWriteBuffers.size())
		{
			secondBuffer = std::move(vecWriteBuffers[1]);
		}
		vecWriteBuffers.clear();
		m_logFilePtr->Flush();
	}
}

#endif