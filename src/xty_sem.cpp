
#include "xty.h"

xtySpinLock xtySemaphore::m_Lock;

xtySemaphore::xtySemaphore(const int InitSemaphore)
{
#ifdef DEBUG
	std::cout << "In xtySemaphore::xtySemaphore()" << std::endl;
#endif
	int fd = open(XTY_SEM_FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	
	if (fd == -1)
		xtyError::ErrorExit(errno);
	
	(void) close(fd);
	
	key_t key = ftok(XTY_SEM_FILE_PATH, XTY_SEM_PROJ_ID);
	
	if (key == -1)
		xtyError::ErrorExit(errno);
	
	m_Lock.Lock();
	
	m_SemID = semget(key, 1, IPC_CREAT);
	
	if (m_SemID == -1)
		xtyError::ErrorExit(errno);
	
	union semun s;
	s.val = InitSemaphore;
		
	if (semctl(m_SemID, 0, SETVAL, s) == -1)
		xtyError::ErrorExit(errno);
	
	m_Lock.UnLock();
}

xtySemaphore::~xtySemaphore()
{
#ifdef DEBUG
	std::cout << "In xtySemaphore::~xtySemaphore()" << std::endl;
#endif
	m_Lock.Lock();
	
	if (semctl(m_SemID, 0, IPC_RMID) == -1)
		xtyError::ErrorExit(errno);
	
	m_Lock.UnLock();
}

int xtySemaphore::Wait()
{
	struct sembuf sem_opt_wait = {0, -1, SEM_UNDO};
	return semop(m_SemID, &sem_opt_wait, 1);
}

int xtySemaphore::WakeUp()
{
	struct sembuf sem_opt_wakeup = {0, 1, SEM_UNDO};
	return semop(m_SemID, &sem_opt_wakeup, 1);
}

