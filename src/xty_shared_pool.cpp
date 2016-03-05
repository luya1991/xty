
#include "xty.h"

xtySpinLock xtySharedPool::m_SingletonLock;
xtySharedPool* xtySharedPool::m_pSharedPool = NULL;

int g_const_proj_id_array[XTY_PROJ_ID_ARRAY_SIZE] =
{0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};

static void* __SharedMemoryAlloc_Aux
(const size_t Size, int* const pSharedMemoryID, xtyProjIDArray* const pProjIDArray)
{
	if (pProjIDArray->m_ProjIDIndex >= XTY_PROJ_ID_ARRAY_SIZE)
	{
		std::cout << "fatal error in __SharedMemoryAlloc_Aux: ";
		std::cout << "pProjIDArray->m_ProjIDIndex is out of range" << std::endl;
		exit(XTY_ERROR); 
	}
	
	int fd = open(XTY_SHM_FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	
	if (fd == -1)
		xtyError::ErrorExit(errno);
	
	(void) close(fd);
	
	pProjIDArray->m_Lock.Lock();
	
	// fatal: same 'key' will lead to same 'shm_id' by 'ftok', while attaching
	// the same 'shm_id' will lose the old shared memory block and cause SIGSEGV!!
	key_t key = ftok(XTY_SHM_FILE_PATH, pProjIDArray->m_ProjIDArray[(pProjIDArray->m_ProjIDIndex)++]);
	
	pProjIDArray->m_Lock.UnLock();
	
	if (key == -1)
		xtyError::ErrorExit(errno);
	
	int shm_id = shmget(key, Size, IPC_CREAT);
	
	if (shm_id == -1)
		xtyError::ErrorExit(errno);

	void* shm_addr = shmat(shm_id, 0, 0);
	
	if ((int) shm_addr == -1)
		xtyError::ErrorExit(errno);
	
	*pSharedMemoryID = shm_id;
	
	return shm_addr;
}

static void __SharedMemoryCleanUp_Aux(const int SharedMemoryID, void* const pSharedMemory)
{
	// fatal: after 'shmdt' 'pSharedMemory' will become invalid,
	// therefore 'pSharedMemory->something' will cause SIGSEGV!!
	if (shmdt(pSharedMemory) == -1)
		xtyError::ErrorExit(errno);
	
	shmid_ds buf;
	
	if (shmctl(SharedMemoryID, IPC_STAT, &buf) == -1)
		xtyError::ErrorExit(errno);

	// 2015-11-19
	// notice: this would be a problem in the future, now 'buf.shm_nattach' will
	// always be '1', so the shared memory block will be removed correctly. but
	// what if somehow 'buf.shm_nattach' is larger than 1 in the future?!	
	if (buf.shm_nattch == 0)
	{
		// if reference count is 0, remove shared memory handle
		if (shmctl(SharedMemoryID, IPC_RMID, 0) == -1)
			xtyError::ErrorExit(errno);
	}
}

xtySharedLargeBlock::xtySharedLargeBlock(const size_t Size, xtyProjIDArray* const pProjIDArray)
: m_LargeSharedMemoryID(0), m_pLargeSharedMemory(NULL), m_pNext(NULL)
{
#ifdef DEBUG
	// std::cout << "In xtySharedLargeBlock::xtySharedLargeBlock()" << std::endl;
#endif
	m_pLargeSharedMemory = __SharedMemoryAlloc_Aux(Size, &m_LargeSharedMemoryID, pProjIDArray);
}

xtySharedLargeBlock::~xtySharedLargeBlock()
{
#ifdef DEBUG
	// std::cout << "In xtySharedLargeBlock::~xtySharedLargeBlock()" << std::endl;
#endif
	xtySharedLargeBlock* pCurrent = this;
	xtySharedLargeBlock* pNext = pCurrent->m_pNext;

	while (pCurrent != NULL)
	{
		__SharedMemoryCleanUp_Aux(pCurrent->m_LargeSharedMemoryID, pCurrent->m_pLargeSharedMemory);
		
		if (pNext == NULL)
			break;
		
		pCurrent = pNext;
		pNext = pNext->m_pNext;
	}
}

xtySharedPoolNode::xtySharedPoolNode(const int SharedMemoryID, void* const pSharedMemory)
: m_SharedMemoryID(SharedMemoryID), m_pSharedMemory(pSharedMemory),
m_pEmptyStart((char*) (pSharedMemory) + sizeof (xtySharedPoolNode)),
m_pEmptyEnd((char*) (pSharedMemory) + XTY_DEFAULT_POOL_SIZE), m_pNext(NULL), m_pLarge(NULL)
{
#ifdef DEBUG
	// std::cout << "In xtySharedPoolNode::xtySharedPoolNode()" << std::endl;
#endif
	/* does nothing */
}

xtySharedPoolNode::~xtySharedPoolNode()
{
#ifdef DEBUG
	// std::cout << "In xtySharedPoolNode::~xtySharedPoolNode()" << std::endl;
#endif
	/* does nothing */
}

xtySharedPool::xtySharedPool(const int SharedMemoryID,
void* const pSharedMemory, xtyProjIDArray* const pProjIDArray)
: m_SharedPoolNode(SharedMemoryID, pSharedMemory), m_pSharedProjIDArray(NULL)
{
#ifdef DEBUG
	std::cout << "In xtySharedPool::xtySharedPool()" << std::endl;
#endif
	// the first node's 'm_pEmptyStart' is a little different from other
	// nodes, the first node's init-offset is 'size-of-xtySharedPool'
	// while other nodes' init-offset is 'size-of-xtySharedPoolNode'
	m_SharedPoolNode.m_pEmptyStart = (char*) (pSharedMemory) + sizeof (xtySharedPool);
	
	// copying 'proj_id' array from outside
	m_pSharedProjIDArray = (xtyProjIDArray*) m_SharedPoolNode.m_pEmptyStart;
	memcpy(m_pSharedProjIDArray, pProjIDArray, sizeof(xtyProjIDArray));
	
	// move 'm_pEmptyStart' by 'xtyProjIDArray' bytes
	m_SharedPoolNode.m_pEmptyStart += sizeof(xtyProjIDArray);
}

xtySharedPool::~xtySharedPool()
{
#ifdef DEBUG
	std::cout << "In xtySharedPool::~xtySharedPool()" << std::endl;
#endif
	/* does nothing */
}

void* xtySharedPool::SharedMemoryAlloc
(const size_t Size, int* pSharedMemoryID, xtyProjIDArray* const pProjIDArray)
{
	return __SharedMemoryAlloc_Aux(Size, pSharedMemoryID, pProjIDArray);
}

void xtySharedPool::SharedMemoryCleanUp(void* pNeedToClean)
{
	if (pNeedToClean == NULL)
		return;
	
	// need to guarantee that 'pNeedToClean' is a 'xtySharedPool*'
	xtySharedPool* pPool = static_cast<xtySharedPool*>(pNeedToClean);
	
	xtySharedPoolNode* pCurrent = &(pPool->m_SharedPoolNode);
	xtySharedPoolNode* pNext = pCurrent->m_pNext;

	while (pCurrent != NULL)
	{	
		if (pCurrent->m_pLarge)
		{
			// does not need 'delete', just 'deconstruction' is fine
			// in order to avoid 'double-free' problem for 'placement-new'
			pCurrent->m_pLarge->~xtySharedLargeBlock();
			pCurrent->m_pLarge = NULL;
		}
		
		__SharedMemoryCleanUp_Aux(pCurrent->m_SharedMemoryID, pCurrent->m_pSharedMemory);
		
		if (pNext == NULL)
			break;
		
		pCurrent = pNext;
		pNext = pNext->m_pNext;
	}
}

void xtySharedPool::Create()
{
	if (m_pSharedPool == NULL)
	{
		m_SingletonLock.Lock();

		// double-check for high efficiency
		if (m_pSharedPool == NULL)
		{
			int SharedMemoryID = 0;
			
			void* pSharedMemory = NULL;
			
			xtyProjIDArray ProjIDArray;
			
			ProjIDArray.m_ProjIDIndex = 0;
			
			// init 'm_ProjIDArray', kinda ugly, because c++ does not allow array-assignment
			memcpy(&ProjIDArray.m_ProjIDArray, &g_const_proj_id_array, sizeof(int) * XTY_PROJ_ID_ARRAY_SIZE);
			
			// does the actual allocation work
			pSharedMemory = SharedMemoryAlloc(XTY_DEFAULT_POOL_SIZE, &SharedMemoryID, &ProjIDArray);
			
			// placement new, does nothing
			m_pSharedPool = new (pSharedMemory) xtySharedPool(SharedMemoryID, pSharedMemory, &ProjIDArray);
		}

		m_SingletonLock.UnLock();
	}
}

void xtySharedPool::Destroy()
{
	if (m_pSharedPool != NULL)
	{
		m_SingletonLock.Lock();

		// double-check for high efficiency
		if (m_pSharedPool != NULL)
		{
			m_pSharedPool->~xtySharedPool();

			SharedMemoryCleanUp(m_pSharedPool);

			m_pSharedPool = NULL;
		}

		m_SingletonLock.UnLock();
	}
}

void* xtySharedPool::DoSharedPoolAllocate(const size_t Size)
{
	// handing large block allocation
	if (Size > XTY_LARGE_BLOCK_SIZE - sizeof(xtySharedPool))
		return DoPoolAllocateLarge(Size);
	
	xtySharedPoolNode* pCurrent = &m_SharedPoolNode;
	xtySharedPoolNode* pLast = pCurrent;
	
	// for 4-bytes aligned allocation
	char* pAligned = NULL;
	
	m_SharedPoolLock.Lock();
	
	// search for a proper pool node to store data
	while (pCurrent != NULL)
	{
		// get 4-bytes aligned ptr
		pAligned = pCurrent->GetAlignedPtr();
		
		// find a proper pool node, which has enougth empty space
		if (Size <= static_cast<size_t>(pCurrent->m_pEmptyEnd - pAligned))
			break;
		
		pLast = pCurrent;
		pCurrent = pCurrent->m_pNext;
	}
	
	// cannot find a proper pool node, create a new one
	if (pCurrent == NULL)
	{
		int SharedMemoryID = 0;
		
		void* pSharedMemory = NULL;
				
		// does the actual allocation work
		pSharedMemory = SharedMemoryAlloc(XTY_DEFAULT_POOL_SIZE, &SharedMemoryID, m_pSharedProjIDArray);
		
		// placement new, does nothing
		pLast->m_pNext = new (pSharedMemory) xtySharedPoolNode(SharedMemoryID, pSharedMemory);
		
		// get the aligned ptr as return-value
		pAligned = pLast->m_pNext->GetAlignedPtr();
		
		// move 'm_pEmptyStart' pointer by 'Size'
		pLast->m_pNext->m_pEmptyStart = pAligned + Size;
	}
	
	// find a proper pool node
	else
		// move 'm_pEmptyStart' pointer by 'Size'
		pCurrent->m_pEmptyStart = pAligned + Size;
	
	m_SharedPoolLock.UnLock();
	
	return pAligned;
}

void* xtySharedPool::DoPoolAllocateLarge(const size_t Size)
{
	void* pTemp = NULL;
	xtySharedLargeBlock* pLarge = NULL;

	// does the actual allocation work
	pTemp = m_pSharedPool->DoSharedPoolAllocate(sizeof(xtySharedLargeBlock));

	// placement new, does nothing
	pLarge = new (pTemp) xtySharedLargeBlock(Size, m_pSharedProjIDArray);
	
	m_SharedPoolLock.Lock();
	
	pLarge->m_pNext = m_SharedPoolNode.m_pLarge;
	m_SharedPoolNode.m_pLarge = pLarge;
	
	m_SharedPoolLock.UnLock();
	
	return pLarge->m_pLargeSharedMemory;
}

void* xtySharedPool::SharedPoolAllocate(const size_t Size)
{
	xtySharedPool* pSharedPool = xtySharedPool::GetInstance();
	
	if(pSharedPool == NULL)
	{
#ifdef DEBUG
		std::cout << "In xtySharedPool::SharedPoolAllocate(): ";
		std::cout << "Need to call xtySharedPool::Create() first" << std::endl;
#endif
		exit(XTY_ERROR);
	}
	
	return pSharedPool->DoSharedPoolAllocate(Size);
}

