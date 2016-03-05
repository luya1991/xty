
#include "xty.h"

xtyCond::xtyCond() : m_NeedToDelete(1)
{
#ifdef DEBUG
	// std::cout << "In xtyCond::xtyCond()" << std::endl;
#endif
	m_pCond = new pthread_cond_t;

	int retval = 0;

	if ((retval = pthread_cond_init(m_pCond, NULL)) != 0)
	{
		delete m_pCond;
		m_pCond = NULL;
		xtyError::ErrorExit("pthread_cond_init() failed", retval);
	}

}

xtyCond::xtyCond(pthread_cond_t* pc) : m_NeedToDelete(0)
{
#ifdef DEBUG
	// std::cout << "In xtyCond::xtyCond()" << std::endl;
#endif
	if (pc == NULL)
		xtyError::ErrorExit("Invalid initialization: Null pthread_cond_t*");

	m_pCond = pc;
}

xtyCond::~xtyCond()
{
#ifdef DEBUG
	// std::cout << "In xtyCond::~xtyCond()" << std::endl;
#endif
	if (m_NeedToDelete == 1)
	{
		if (m_pCond != NULL)
		{
			(void) pthread_cond_destroy(m_pCond);
			delete m_pCond;
			m_pCond = NULL;
		}
	}
}

int xtyCond::Wait(xtyMutex* pMutex)
{
	return pthread_cond_wait(m_pCond, pMutex->GetRawMutexPtr());
}

int xtyCond::WakeUp()
{
	return pthread_cond_signal(m_pCond);
}

int xtyCond::WakeUpAll()
{
	return pthread_cond_broadcast(m_pCond);
}

