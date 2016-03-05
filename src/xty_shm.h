
#ifndef XTY_SHM_H
#define XTY_SHM_H

// ftok (1~255)
#define XTY_SHM_PROJ_ID 1

// default shm size
#define XTY_SHM_DEFAULT_SIZE 4096

// shm file path
#define XTY_SHM_FILE_PATH \
"/home/genius/workspace/proj/xty/tmp/xty.ipc"

class xtySharedMemory
{
public:
	explicit xtySharedMemory(size_t AllocBytes = XTY_SHM_DEFAULT_SIZE);
	~xtySharedMemory();

private:
	xtySharedMemory(const xtySharedMemory&);
	xtySharedMemory& operator = (const xtySharedMemory&);

public:
	// get shared memory's addr
	inline void* GetSharedMemory() { return m_pShmAddr; }
	
	// get shared memory's ref-count 
	inline int GetReferenceCount()
	{
		shmid_ds buf;
	
		if (shmctl(m_ShmID, IPC_STAT, &buf) == -1)
			xtyError::ErrorExit(errno);

		return buf.shm_nattch;
	}

public:
#ifdef DEBUG
	friend class xtySharedMemoryTest;
#endif

private:
	// shared memory handle
	int m_ShmID;
	
	// shared memory address
	void* m_pShmAddr;
	
	/* 
	 * why we need this spinlock?
	 * 
	 * case in point:
	 * threadA: return from shmget() with a handle, about to call shmat();
	 * threadB: about to call shmctl(IPC_RMID);
	 *
	 * if B goes first, the handle in A will be dangling. 
	 */
	
	// lock up during creation/destruction
	static xtySpinLock m_Lock;
};

#endif

