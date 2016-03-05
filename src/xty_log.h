
#ifndef XTY_LOG_H
#define XTY_LOG_H

// add '\n' & '\0' to be 4096 
#define XTY_LOG_MSG_LEN 4094

#define XTY_LOG_FILE_PATH \
"/home/genius/workspace/proj/xty/tmp/xty.log"

class xtyLog
{
private:
	// does not allow con/decon
	xtyLog();
	~xtyLog();

private:
	// does not allow copying
	xtyLog(const xtyLog&);
	xtyLog& operator = (const xtyLog&);

public:
	// get singleton pointer 
	inline static xtyLog* GetInstance() { return m_pLog; }
	
	// only 1 interface for outside-class accessing
	static int WriteLog(const char* fmt, ...);

private:
	// singleton create function, called at the start of a program
	static void Create();
	
	// singleton destroy function, called at the end of a program
	static void Destroy();
	
	// auxiliary method for 'WriteLog', does the real write-log work
	int DoWriteLog(const char* fmt, va_list args);

public:
#ifdef DEBUG
	friend class xtyLogTest;
#endif

public:
	// added on 2015-11-11, for 'xtyFundamentalFacility'
	friend class xtyFundamentalFacility;

private:
	// log file descriptor
	int m_FileDescriptor;

	// singleton members
	static xtySpinLock m_Lock;
	static xtyLog* m_pLog;
};

#endif

