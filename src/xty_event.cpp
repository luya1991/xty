
#include "xty.h"

int xtyThreadEvent::Produce()
{
	m_Lock.Lock();
	
	m_Events++;
	std::cout << "In xtyThreadEvent::Produce(): " << m_Events << std::endl;
	
	m_Lock.UnLock();
	
	if (m_Notifier.WakeUp() != XTY_OK)
		xtyError::ErrorExit(errno);

	return XTY_OK;
}

int xtyThreadEvent::Consume()
{
	m_Lock.Lock();
	
	while(m_Events == 0)
	{
		if (m_Notifier.Wait(&m_Lock) != XTY_OK)
			xtyError::ErrorExit(errno);
	}
	
	m_Events--;
	std::cout << "In xtyThreadEvent::Consume(): " << m_Events << std::endl;
	
	m_Lock.UnLock();
	return XTY_OK;
}

int xtyProcessEvent::Produce()
{
	m_Lock.Lock();
	
	m_Events++;
	std::cout << "In xtyProcessEvent::Produce(): " << m_Events << std::endl;
	
	m_Lock.UnLock();
	
	if (m_Notifier.WakeUp() != XTY_OK)
		xtyError::ErrorExit(errno);

	return XTY_OK;
}

int xtyProcessEvent::Consume()
{	
	while(m_Events == 0)
	{
		if (m_Notifier.Wait() != XTY_OK)
			xtyError::ErrorExit(errno);
	}
	
	m_Lock.Lock();
	
	m_Events--;
	std::cout << "In xtyProcessEvent::Consume(): " << m_Events << std::endl;
	
	m_Lock.UnLock();
	return XTY_OK;
}

