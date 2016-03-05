
#ifndef XTY_JOB_H
#define XTY_JOB_H

class xtyJob
{
public:
	xtyJob()
	{/* does nothing but need a definition */}
	
	virtual ~xtyJob()
	{/* does nothing but need a definition */}

private:
	xtyJob(const xtyJob&);
	xtyJob& operator = (const xtyJob&);
	
public:
	virtual void RunJobRoutine(void* pContext = NULL) = 0;
};

class xtyDullJob : public xtyJob
{
public:
	xtyDullJob()
	{/* does nothing but need a definition */}
	
	virtual ~xtyDullJob()
	{/* does nothing but need a definition */}

private:
	xtyDullJob(const xtyDullJob&);
	xtyDullJob& operator = (const xtyDullJob&);
	
public:
	virtual void RunJobRoutine(void* pContext = NULL)
	{ std::cout << "In xtyDullJob::RunJobRoutine()" << std::endl; }
};

#endif

