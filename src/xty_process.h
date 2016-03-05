
#ifndef XTY_PROCESS_H
#define XTY_PROCESS_H

class xtyProcess : public xtyWorker
{
public:
	explicit xtyProcess(const std::string& Name);
	virtual ~xtyProcess();

private:
	xtyProcess(const xtyProcess&);
	xtyProcess& operator = (const xtyProcess&);
	
public:
	// start worker run routine
	virtual int Run(void* pContext = 0);
	
	// quit worker run routine, normal mode
	virtual int Quit();
	
	// quit worker run routine, signal interruption mode
	virtual int QuitNoDelay();

private:
	void* m_pContext;
	pid_t m_ProcessID;
};

#endif

