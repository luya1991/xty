
#ifndef XTY_THREAD_H
#define XTY_THREAD_H

class xtyThread : public xtyWorker
{
public:
	explicit xtyThread(const std::string& Name);
	virtual ~xtyThread();

private:
	xtyThread(const xtyThread&);
	xtyThread& operator = (const xtyThread&);
	
public:
	// start worker run routine
	virtual int Run(void* pContext = 0);
	
	// quit worker run routine, normal mode
	virtual int Quit();
	
	// quit worker run routine, signal interruption mode
	virtual int QuitNoDelay();
	
private:
	// signal handler for 'SIGQUIT'
	static void SIGQUIT_Handler(int signo);
	
	// 'void* func(void*)', the entrance of thread run routine
	static void* ThreadRunRoutine(void* pContext);
	
private:
	void* m_pContext;
	pthread_t m_ThreadID;
};

#endif

