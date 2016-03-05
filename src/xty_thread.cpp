
#include "xty.h"

xtyThread::xtyThread(const std::string& Name)
: xtyWorker(Name)
{
	/* does nothing but need a definition */
}

xtyThread::~xtyThread()
{
	/* does nothing but need a definition */
}

int xtyThread::Run(void* pContext)
{
	m_pContext = pContext;
	
	int retval = 0;

	// because I have to use 'm_pJob' in 'ThreadRunRoutune', I pass 'this' as arg
	if ((retval = pthread_create(&m_ThreadID, NULL, ThreadRunRoutine, (void*)this)) != XTY_OK)
		xtyError::ErrorExit("In xtyThread::Run(): pthread_create() failed", retval);
	
	return XTY_OK;
}

int xtyThread::Quit()
{
	int retval = 0;

	if ((retval = pthread_join(m_ThreadID, NULL)) != XTY_OK)
		xtyError::ErrorExit("In xtyThread::Quit(): pthread_join() failed", retval);
	
	return XTY_OK;
}

int xtyThread::QuitNoDelay()
{
	int retval = 0;

	if ((retval = pthread_kill(m_ThreadID, SIGQUIT)) != XTY_OK)
		xtyError::ErrorExit("In xtyThread::QuitNoDelay(): pthread_kill() failed", retval);
	
	if ((retval = pthread_join(m_ThreadID, NULL)) != XTY_OK)
		xtyError::ErrorExit("In xtyThread::QuitNoDelay(): pthread_join() failed", retval);

	return XTY_OK;
}

void xtyThread::SIGQUIT_Handler(int signo)
{
	// simply exit the thread
	pthread_exit(NULL);
}

void* xtyThread::ThreadRunRoutine(void* pContext)
{
	struct sigaction act, oact;

	act.sa_handler = SIGQUIT_Handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	// add signal handler for current thread
	sigaction(SIGQUIT, &act, &oact);
	
	// have to make sure that 'pContext' is a 'this'
	xtyThread* pThis = (xtyThread*) pContext;
	
	// have to make sure that 'm_pJob' is non-null
	if (pThis->m_pJob == NULL)
		xtyError::ErrorExit("In xtyThread::ThreadRunRoutine(): need to load job first");

	// start job run routine
	pThis->m_pJob->RunJobRoutine(pThis->m_pContext);
	
	// quit thread
	pthread_exit(NULL);
}

