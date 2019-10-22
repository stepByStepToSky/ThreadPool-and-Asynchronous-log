#ifndef MUTEX_H
#define MUTEX_H

#pragma once

#include <iostream>
#include <pthread.h>

class Mutex
{
public:
	Mutex()
	{
		pthread_mutex_init(&m_mutex, NULL);
	}
	
	Mutex(const Mutex &) = delete;
	Mutex & operator= (const Mutex &) = delete;
	
	~Mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}
	
	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	
	void Unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
	
	bool Trylock()
	{
		return EBUSY != pthread_mutex_trylock(&m_mutex); 
	}
	
	pthread_mutex_t * GetMutex()
	{
		return &m_mutex;
	}

private:
	pthread_mutex_t m_mutex;
};




class LockGuard
{
public:
	LockGuard(Mutex & mutex):m_mutex(mutex)
	{
		m_mutex.Lock();
	}
	
	~LockGuard()
	{
		m_mutex.Unlock();
	}
	
private:
	Mutex & m_mutex;
};
#endif