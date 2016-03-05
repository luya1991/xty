
#ifndef XTY_WORKER_H
#define XTY_WORKER_H

class xtyWorker
{
public:
	explicit xtyWorker(const std::string& Name) : m_Name(Name), m_pJob(NULL)
	{ /* does nothing but need a definition */ }
	
	virtual ~xtyWorker()
	{ /* does nothing but need a definition */ }

private:
	xtyWorker(const xtyWorker&);
	xtyWorker& operator = (const xtyWorker&);
		
public:
	// load job, inherits 'xtyJob*' from outside
	inline void LoadJob(xtyJob* pJob)
	{
		if (pJob == NULL)
			xtyError::ErrorExit("In xtyWorker::LoadJob(): cannot load job with null xtyJob*");
		
		m_pJob = pJob;
	}

public:
	// start worker run routine
	virtual int Run(void* pContext = 0) = 0;
	
	// quit worker run routine, normal mode
	virtual int Quit() = 0;
	
	// quit worker run routine, signal interruption mode
	virtual int QuitNoDelay() = 0;
	
protected:
	std::string m_Name;
	xtyJob* m_pJob;
};

class xtyDullWorker : public xtyWorker
{
public:
	explicit xtyDullWorker(const std::string& Name) : xtyWorker(Name)
	{ /* does nothing but need a definition */ }
	
	virtual ~xtyDullWorker()
	{ /* does nothing but need a definition */ }

private:
	xtyDullWorker(const xtyDullWorker&);
	xtyDullWorker& operator = (const xtyDullWorker&);

public:
	virtual inline int Run(void* pContext = 0)
	{
		// have to make sure that 'm_pJob' is non-null
		if (m_pJob == NULL)
			xtyError::ErrorExit("In xtyDullWorker::Run(): need to load job first");

		// start job routine
		m_pJob->RunJobRoutine(pContext);

		return XTY_OK;
	}
	
	virtual inline int Quit()
	{
		return XTY_OK;
	}

	virtual inline int QuitNoDelay()
	{
		return XTY_OK;
	}
};

#endif

