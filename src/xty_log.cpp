
#include "xty.h"

xtySpinLock xtyLog::m_Lock;
xtyLog* xtyLog::m_pLog = NULL;

xtyLog::xtyLog()
{
#ifdef DEBUG
	std::cout << "In xtyLog::xtyLog()" << std::endl;
#endif
	// simply open fd
	m_FileDescriptor = open(XTY_LOG_FILE_PATH, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
	
	if (m_FileDescriptor == -1)
	{
#ifdef DEBUG
		std::cout << strerror(errno) << " errno: " << errno << std::endl;
#endif
		exit(XTY_ERROR);
	}
}

xtyLog::~xtyLog()
{
#ifdef DEBUG
	std::cout << "In xtyLog::~xtyLog()" << std::endl;
#endif
	// simply close fd
	(void) close(m_FileDescriptor);	
}

void xtyLog::Create()
{
	if (m_pLog == NULL)
	{
		m_Lock.Lock();

		// double-check for high efficiency
		if (m_pLog == NULL)
			m_pLog = new xtyLog;

		m_Lock.UnLock();
	}
}

void xtyLog::Destroy()
{
	if (m_pLog != NULL)
	{
		m_Lock.Lock();

		// double-check for high efficiency
		if (m_pLog != NULL)
		{
			delete m_pLog;
			m_pLog = NULL;
		}

		m_Lock.UnLock();
	}
}

int xtyLog::WriteLog(const char* fmt, ...)
{
#ifdef DEBUG
	// std::cout << "In xtyLog::WriteLog()" << std::endl;
#endif
	xtyLog* pLog = GetInstance();
	
	// check whether 'pLog' is NULL or not
	if(pLog == NULL)
	{
#ifdef DEBUG
		std::cout << "In xtyLog::WriteLog(): ";
		std::cout << "Need to call xtyLog::Create() first" << std::endl;
#endif
		exit(XTY_ERROR);
	}
	
	va_list	args;
	va_start(args, fmt);
	int ret = pLog->DoWriteLog(fmt, args);
	va_end(args);
	
	return ret;
}

int xtyLog::DoWriteLog(const char* fmt, va_list args)
{
#ifdef DEBUG
	// std::cout << "In xtyLog::DoWriteLog()" << std::endl;
#endif
	char buf[XTY_LOG_MSG_LEN + 2] = {0};
	
	// the actual log information's max size is 4094 bytes,
	// with '\n' and '\0' at the end making the total size to be 4096 bytes
	vsnprintf(buf, XTY_LOG_MSG_LEN, fmt, args);
	
	strcat(buf, "\n");
	strcat(buf, "\0");
	
	int len = strlen(buf);
	
	// 2015-06-09 abandoned
	// multi-process/multi-thread lock seems to be needless 
	// since the log file will be locked up during 'write()'
	
	// multi-process/multi-thread lock
	// m_Lock.Lock();
	
	int write_failed = 0;
	struct flock lock;

	// lock-bytes-field: (from 'SEEK_END' to whatever bytes will be written)	
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	// lock file content
	if(fcntl(m_FileDescriptor, F_SETLKW, &lock) == -1)
	{
#ifdef DEBUG
		std::cout << strerror(errno) << " errno: " << errno << std::endl;
#endif
		exit(XTY_ERROR);
	}
	
	// write log msg from buf to file
	ssize_t writebytes = write(m_FileDescriptor, buf, len);
	
	if(writebytes == -1)
	{
#ifdef DEBUG
		std::cout << strerror(errno) << " errno: " << errno << std::endl;
#endif
		exit(XTY_ERROR);
	}
	
	// unlock-bytes-field: (from old 'SEEK_END' to new 'SEEK_END')
	lock.l_type = F_UNLCK;
	lock.l_start = -writebytes;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	// unlock file content
	(void) fcntl(m_FileDescriptor, F_SETLKW, &lock);
	
	return XTY_OK;
	
	// 2015-06-09 abandoned
	// multi-process/multi-thread lock seems to be needless 
	// since the log file will be locked up during 'write()'
	
	// multi-process/multi-thread unlock
	// m_Lock.UnLock();
}

