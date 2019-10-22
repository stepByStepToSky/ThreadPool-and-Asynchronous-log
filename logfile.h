#ifndef LOGFILE
#define LOGFILE

#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <memory>
#include <string>
#include <iostream>
#include "writefile.h"

class LogFile
{
public:
	LogFile()
	{
		RollFile();
	}
	~LogFile()
	{
		Close();
	}
	
	LogFile(const LogFile &) = delete;
	LogFile & operator= (const LogFile &) = delete;
	
	void Append(const char * data, size_t writeSize);
	void Flush();
	void Sync();
	void Close();
	
private:
	void GetFileName(time_t nowTime, std::string & sLogName);
	void RollFile();
	
private:
	std::unique_ptr<WriteFile> m_ptWriteFile;
	// do m_cnt append operators, check if we need a flush to garantee the data safely, and check if we need roll the log file
	size_t m_cnt;
	size_t m_writedSize;
	time_t m_lastFlushTime;
	time_t m_lastRollTime;
	static const time_t kRollPerSeconds = 24 * 60 * 60;
	static const size_t kRollFileSize = 64 * 1024 * 1024;
	static const size_t kCheckNSteps = 1000;
	static const size_t kFlushInteralSeconds = 3;
};

void LogFile::GetFileName(time_t nowTime, std::string & sLogName)
{
	struct tm * stTime = gmtime(&nowTime);
	sLogName.resize(19);
	strftime(const_cast<char *> (sLogName.data()), sLogName.capacity(), "%Y%m%d-%H%M%S.log", stTime);
}

void LogFile::RollFile()
{
	time_t nowTime = time(NULL);
	std::string sLogName;
	GetFileName(nowTime, sLogName);
	m_ptWriteFile.reset(new WriteFile(sLogName));
	m_cnt = 0;
	m_writedSize = 0;
	m_lastFlushTime = nowTime;
	m_lastRollTime = nowTime;
}

void LogFile::Append(const char * data, size_t writeSize)
{
	// FIX ME if writeSize is larger than kRollFileSize, will cause wrong
	if (m_writedSize > kRollFileSize - writeSize)
	{
		//std::cout << "RollFile " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << m_writedSize << " " << std::endl; 
		RollFile();
	}
	
	m_ptWriteFile->Append(data, writeSize);
	m_writedSize += writeSize;
	m_cnt += 1;
	
	//std::cout << __FILE__ << " " << __LINE__ << " " << __func__ << " " << m_cnt << " " << m_writedSize << std::endl; 
	
	if (kCheckNSteps < m_cnt)
	{
		time_t nowTime = time(NULL);
		time_t nextStartTime = nowTime / kRollPerSeconds * kRollPerSeconds;
		// next day shoule roll log file
		if (m_lastRollTime < nextStartTime)
		{
			//std::cout << "RollFile " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << nowTime << " " << m_lastFlushTime << std::endl; 
			RollFile();
		}
		else if (kFlushInteralSeconds < nowTime - m_lastFlushTime)
		{
			//std::cout << "Flush " << __FILE__ << " " << __LINE__ << " " << __func__ << " " << nowTime << " " << m_lastFlushTime << std::endl; 
			Flush();
			m_lastFlushTime = nowTime;
		}
		
		m_cnt = 0;
	}
}

void LogFile::Flush()
{
	m_ptWriteFile->Flush();
}

void LogFile::Sync()
{
	m_ptWriteFile->Sync();
}

void LogFile::Close()
{
	m_ptWriteFile->Close();
}


#endif