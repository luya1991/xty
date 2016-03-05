
#include "xty.h"

xtySpinLock::xtySpinLock() : m_Atomic(0)
{
#ifdef DEBUG
	// std::cout << "In xtySpinLock::xtySpinLock()" << std::endl;
#endif
}

xtySpinLock::~xtySpinLock()
{
#ifdef DEBUG
	// std::cout << "In xtySpinLock::~xtySpinLock()" << std::endl;
#endif
}

int xtySpinLock::Lock()
{
#ifdef DEBUG
	// std::cout << "In xtySpinLock::Lock()" << std::endl;
#endif
	volatile int lock_ok = 0;
	
	while (lock_ok == 0)
	{
		// if lock is '0', try to set lock to be '1'
		lock_ok = __sync_bool_compare_and_swap(&m_Atomic, 0, 1);
	}
	
	return XTY_OK;
}

int xtySpinLock::TryLock()
{
#ifdef DEBUG
	// std::cout << "In xtySpinLock::TryLock()" << std::endl;
#endif
	// if lock is '0', try to set lock to be '1'
	if (__sync_bool_compare_and_swap(&m_Atomic, 0, 1))
		return XTY_OK;
	else
		return XTY_ERROR;
}

void xtySpinLock::UnLock()
{
#ifdef DEBUG
	// std::cout << "In xtySpinLock::UnLock()" << std::endl;
#endif
	// set lock to be '0'
	__sync_lock_release(&m_Atomic);
}
