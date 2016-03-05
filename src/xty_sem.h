
#ifndef XTY_SEM_H
#define XTY_SEM_H

// ftok (1~255)
#define XTY_SEM_PROJ_ID 1

// default semaphore value
#define XTY_SEM_DEFAULT_VAL 0

// semaphore ipc file path
#define XTY_SEM_FILE_PATH \
"/home/genius/workspace/proj/xty/tmp/xty.ipc"

/*
 * 'union semun' is defined in 'linux/sem.h'.
 * if we include 'linux/sem.h', there will be
 * some definition-conflict since we have
 * included 'sys/sem.h' already.
 * instead of 'include', we copy the definition
 * of 'union semun' from 'linux/sem.h' as below.
 */

union semun
{
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};
   
class xtySemaphore
{
public:
	explicit xtySemaphore(const int InitSemaphore = XTY_SEM_DEFAULT_VAL);
	~xtySemaphore();
	
private:
	xtySemaphore(const xtySemaphore&);
	xtySemaphore& operator = (const xtySemaphore&);

public:
	// p-operation
	int Wait();
	
	// v-operation
	int WakeUp();

#ifdef DEBUG
	friend class xtySemaphoreTest;
#endif

private:
	// semaphore handle
	int m_SemID;
	
	// lock up during creation/destruction
	// (for more details please refer to 'xty_shm.h')
	static xtySpinLock m_Lock;
};

#endif

