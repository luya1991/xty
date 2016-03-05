
#include "xty.h"

xtyMutex::xtyMutex() : m_NeedToDelete(1)
{
#ifdef DEBUG
	// std::cout << "In xtyMutex::xtyMutex()" << std::endl;
#endif
	m_pMutex = new pthread_mutex_t;
	
	int retval = 0;

	if ((retval = pthread_mutex_init(m_pMutex, NULL)) != 0)
	{
		delete m_pMutex;
		m_pMutex = NULL;
		xtyError::ErrorExit("pthread_mutex_init() failed", retval);
	}

}

xtyMutex::xtyMutex(pthread_mutex_t* pm) : m_NeedToDelete(0)
{
#ifdef DEBUG
	// std::cout << "In xtyMutex::xtyMutex()" << std::endl;
#endif
	if (pm == NULL)
		xtyError::ErrorExit("Invalid initialization: Null pthread_mutex_t*");
	
	m_pMutex = pm;
}

xtyMutex::~xtyMutex()
{
#ifdef DEBUG
	// std::cout << "In xtyMutex::~xtyMutex()" << std::endl;
#endif
	if (m_NeedToDelete == 1)
	{
		if (m_pMutex != NULL)
		{
			(void) pthread_mutex_destroy(m_pMutex);
			delete m_pMutex;
			m_pMutex = NULL;
		}
	}
}

int xtyMutex::Lock()
{
	return pthread_mutex_lock(m_pMutex);
}

int xtyMutex::TryLock()
{
	return pthread_mutex_trylock(m_pMutex);
}

void xtyMutex::UnLock()
{
	(void) pthread_mutex_unlock(m_pMutex);
}

