#ifndef WRITEFILE_H
#define WRITEFILE_H

#pragma once

#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>

class WriteFile
{
public:
	WriteFile(std::string sFilePath) : m_pos(0)
	{
		m_fd = open(sFilePath.c_str(), O_RDWR | O_APPEND | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	};
	~WriteFile()
	{
		if (0 <= m_fd)
		{
			Close();
		}
	}
	WriteFile(const WriteFile &) = delete;
	WriteFile & operator= (const WriteFile &) = delete;
	
	void Append(const char * data, size_t writeSize);
	void Flush();
	void Sync();
	void Close()
	{
		Sync();
		close(m_fd);
		m_fd = -1;
	}
	
private:
	void WriteUnbuffered(const char * data, size_t writeSize);
	
private:
	static const size_t kWriteFileBufferSize = 65536;
	
	int m_fd;
	// m_buff[0, m_pos - 1] contains data to be written to m_fd.
	char m_buff[kWriteFileBufferSize];
	size_t m_pos;
};

void WriteFile::Append(const char * data, size_t writeSize)
{
	// feed as much as possible into the buffer
	size_t nCanWrited = std::min(writeSize, (kWriteFileBufferSize - m_pos));
	memcpy(m_buff + m_pos, data, nCanWrited);
	writeSize -= nCanWrited;
	data += nCanWrited;
	m_pos += nCanWrited;
	if (0 == writeSize)
	{
		return;
	}
	
	// can't feed all data into buffer, need to do write m_fd
	Flush();
	
	// rest data can be writed into m_buff, then write them into m_bufff, else do write m_fd directly
	if (writeSize < kWriteFileBufferSize)
	{
		memcpy(m_buff, data, writeSize);
		m_pos = writeSize;
		return;
	}
	
	WriteUnbuffered(data, writeSize);
}

void WriteFile::Flush()
{
	WriteUnbuffered(m_buff, m_pos);
	m_pos = 0;
}


void WriteFile::WriteUnbuffered(const char * data, size_t writeSize)
{
	while(0 < writeSize)
	{
		ssize_t nWriten = write(m_fd, data, writeSize);
		if (0 > nWriten)
		{
			// retry
			if (EINTR == errno)
			{
				continue;
			}
			return;
		}
		
		writeSize -= nWriten;
		data += nWriten;
	}
}


void WriteFile::Sync()
{
	Flush();
	fdatasync(m_fd);
}

#endif