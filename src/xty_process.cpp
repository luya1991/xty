
#include "xty.h"

xtyProcess::xtyProcess(const std::string& Name)
: xtyWorker(Name)
{
	/* does nothing but need a definition */
}

xtyProcess::~xtyProcess()
{
	/* does nothing but need a definition */
}

int xtyProcess::Run(void* pContext)
{
	m_pContext = pContext;
	
	// 'vfork': parent and child would share one copy of
	// address-space, could avoid copying behavior by 'fork'
	m_ProcessID = vfork();
	
	if (m_ProcessID == 0)
	{	
		// have to make sure that 'm_pJob' is non-null
		if (m_pJob == NULL)
			xtyError::ErrorExit("In xtyProcess::Run(): need to load job first");

		// call 'exec' inside, if successes, the following code will not be executed
		m_pJob->RunJobRoutine(pContext);
		
		// some error must happen in 'exec'
		xtyError::ErrorExit("In xtyProcess::Run(): exec() failed", errno);
	}
	
	else if (m_ProcessID > 0)
	{
		// the job is delivered to child proc, therefore parent proc does nothing
	}
	
	else
	{
		// some error happens in 'vfork'
		xtyError::ErrorExit("In xtyProcess::Run(): vfork() failed", errno);
	}
	
	return XTY_OK;
}

int xtyProcess::Quit()
{
	if (waitpid(m_ProcessID, NULL, 0) != XTY_OK)
		xtyError::ErrorExit("In xtyProcess::Quit(): waitpid() failed", errno);
	
	return XTY_OK;
}

int xtyProcess::QuitNoDelay()
{
	if (kill(m_ProcessID, SIGQUIT) != XTY_OK)
		xtyError::ErrorExit("In xtyProcess::QuitNoDelay(): kill() failed", errno);

	if (waitpid(m_ProcessID, NULL, 0) != XTY_OK)
		xtyError::ErrorExit("In xtyProcess::QuitNoDelay(): waitpid() failed", errno);
	
	return XTY_OK;
}

