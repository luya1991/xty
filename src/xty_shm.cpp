
#include "xty.h"

xtySpinLock xtySharedMemory::m_Lock;

xtySharedMemory::xtySharedMemory(size_t AllocBytes)
{
#ifdef DEBUG
	std::cout << "In xtySharedMemory::xtySharedMemory()" << std::endl;
#endif
	int fd = open(XTY_SHM_FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	
	if (fd == -1)
		xtyError::ErrorExit(errno);
	
	(void) close(fd);
	
	key_t key = ftok(XTY_SHM_FILE_PATH, XTY_SHM_PROJ_ID);
	
	if (key == -1)
		xtyError::ErrorExit(errno);
	
	m_Lock.Lock();
	
	m_ShmID = shmget(key, AllocBytes, IPC_CREAT);
	
	if (m_ShmID == -1)
		xtyError::ErrorExit(errno);

	m_pShmAddr = shmat(m_ShmID, 0, 0);
	
	if ((int)m_pShmAddr == -1)
		xtyError::ErrorExit(errno);
	
	m_Lock.UnLock();
}

xtySharedMemory::~xtySharedMemory()
{
#ifdef DEBUG
	std::cout << "In xtySharedMemory::~xtySharedMemory()" << std::endl;
#endif
	m_Lock.Lock();
	
	if (shmdt(m_pShmAddr) == -1)
		xtyError::ErrorExit(errno);
	
	if (GetReferenceCount() == 0)
	{	
		// if reference count is 0, remove shared memory handle
		if (shmctl(m_ShmID, IPC_RMID, 0) == -1)
			xtyError::ErrorExit(errno);
	}
	
	m_Lock.UnLock();
}

