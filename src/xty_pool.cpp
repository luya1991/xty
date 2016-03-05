
#include "xty.h"

xtySpinLock xtyPool::m_SingletonLock;
xtyPool* xtyPool::m_pPool = NULL;

xtyLargeBlock::xtyLargeBlock(const size_t Size) : m_pData(NULL), m_pNext(NULL)
{
#ifdef DEBUG
	// std::cout << "In xtyLargeBlock::xtyLargeBlock()" << std::endl;
#endif
	m_pData = malloc(Size);
	
	if (m_pData == NULL)
		xtyError::ErrorExit("In xtyLargeBlock::xtyLargeBlock(): malloc error", errno);
}

xtyLargeBlock::~xtyLargeBlock()
{
#ifdef DEBUG
	// std::cout << "In xtyLargeBlock::~xtyLargeBlock()" << std::endl;
#endif
	xtyLargeBlock* pLarge;
	
	for (pLarge = this; pLarge; pLarge = pLarge->m_pNext)
	{
		if (pLarge->m_pData)
		{
			free(pLarge->m_pData);
			pLarge->m_pData = NULL;
		}
	}
}

xtyPoolNode::xtyPoolNode(void* const pPoolAddr)
: m_pEmptyStart((char*) (pPoolAddr) + sizeof (xtyPoolNode)),
m_pEmptyEnd((char*) (pPoolAddr) + XTY_DEFAULT_POOL_SIZE), m_pNext(NULL), m_pLarge(NULL)
{
#ifdef DEBUG
	// std::cout << "In xtyPoolNode::xtyPoolNode()" << std::endl;
#endif
	/* does nothing */
}

xtyPoolNode::~xtyPoolNode()
{
#ifdef DEBUG
	// std::cout << "In xtyPoolNode::~xtyPoolNode()" << std::endl;
#endif
	/* does nothing */
}

xtyPool::xtyPool(void* const pPoolAddr)
: m_PoolNode(pPoolAddr)
{
#ifdef DEBUG
	std::cout << "In xtyPool::xtyPool()" << std::endl;
#endif
	// the first node's 'm_pEmptyStart' is a little different from
	// other nodes, the first node's init-offset is 'size-of-xtyPool',
	// while other nodes' init-offset is 'size-of-xtyPoolNode'
	m_PoolNode.m_pEmptyStart = (char*) (pPoolAddr) + sizeof (xtyPool);
}

xtyPool::~xtyPool()
{
#ifdef DEBUG
	std::cout << "In xtyPool::~xtyPool()" << std::endl;
#endif
	/* does nothing */
}

void* xtyPool::MemoryAlignAlloc(const size_t Alignment)
{
	void* pAlignedMemory = NULL;
	
	if (posix_memalign(&pAlignedMemory, Alignment, XTY_DEFAULT_POOL_SIZE) != 0)
		xtyError::ErrorExit(errno);
	
	return pAlignedMemory;
}

void xtyPool::MemoryCleanUp(void* pNeedToClean)
{
	if (pNeedToClean == NULL)
		return;
	
	// need to guarantee that 'pNeedToClean' is a 'xtyPool*'
	xtyPool* pPool = static_cast<xtyPool*>(pNeedToClean);
	
	xtyPoolNode* pCurrent = &(pPool->m_PoolNode);
	xtyPoolNode* pNext = pCurrent->m_pNext;

	while (pCurrent != NULL)
	{
		/* Notice: only the first xtyPoolNode's member 'm_pLarge' is in use, others are all null */
		
		// first, clean up the large blocks (xtyLargeBlock)
		if (pCurrent->m_pLarge)
		{
			// does not need 'delete', just 'deconstruction' is fine
			// in order to avoid 'double-free' problem for 'placement-new'
			pCurrent->m_pLarge->~xtyLargeBlock();
			pCurrent->m_pLarge = NULL;
		}
		
		// then, clean up all the normal blocks (xtyPoolNode)
		// (pitfall: except for the first block, because it is not heap-allocated)
		if (pCurrent != &(pPool->m_PoolNode))
			free(pCurrent);
		
		if (pNext == NULL)
			break;
		
		pCurrent = pNext;
		pNext = pNext->m_pNext;
	}
	
	// finally, clean up the whole pool (xtyPool)
	// (notice: actually the first node's addr is equal to pool's addr
	// however, the first node is not heap-allocated while the pool is)
	free(pPool);
}

void xtyPool::Create()
{
	if (m_pPool == NULL)
	{
		m_SingletonLock.Lock();

		// double-check for high efficiency
		if (m_pPool == NULL)
		{
			// does the actual memory allocation work
			void* pTemp = MemoryAlignAlloc(XTY_POOL_ALIGNMENT);

			// placement new, does nothing
			m_pPool = new (pTemp) xtyPool(pTemp);
		}

		m_SingletonLock.UnLock();
	}
}

void xtyPool::Destroy()
{
	if (m_pPool != NULL)
	{
		m_SingletonLock.Lock();

		// double-check for high efficiency
		if (m_pPool != NULL)
		{
			// does not need 'delete', just 'deconstruction' is fine
			// in order to avoid 'double-free' problem for 'placement-new'
			m_pPool->~xtyPool();

			// does the actual cleaning-up work
			MemoryCleanUp(m_pPool);

			// reset pool pointer to be null
			m_pPool = NULL;
		}

		m_SingletonLock.UnLock();
	}
}

void* xtyPool::DoPoolAllocate(const size_t Size)
{
	// handing large block allocation
	if (Size > XTY_LARGE_BLOCK_SIZE - sizeof(xtyPool))
		return DoPoolAllocateLarge(Size);
	
	xtyPoolNode* pCurrent = &m_PoolNode;
	xtyPoolNode* pLast = pCurrent;
	
	// for 4-bytes aligned allocation
	char* pAligned = NULL;
	
	m_PoolLock.Lock();
	
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
		void* pTemp = NULL;
		
		// does the actual allocation work
		pTemp = MemoryAlignAlloc(XTY_POOL_ALIGNMENT);
		
		// placement new, does nothing
		pLast->m_pNext = new (pTemp) xtyPoolNode(pTemp);
		
		// get the aligned ptr as return-value
		pAligned = pLast->m_pNext->GetAlignedPtr();
		
		// move 'm_pEmptyStart' pointer by 'Size'
		pLast->m_pNext->m_pEmptyStart = pAligned + Size;
	}
	
	// find a proper pool node
	else
		// move 'm_pEmptyStart' pointer by 'Size'
		pCurrent->m_pEmptyStart = pAligned + Size;
	
	m_PoolLock.UnLock();
	
	return pAligned;
}

void* xtyPool::DoPoolAllocateLarge(const size_t Size)
{
	void* pTemp = NULL;
	xtyLargeBlock* pLarge = NULL;

	// does the actual allocation work
	pTemp = m_pPool->DoPoolAllocate(sizeof(xtyLargeBlock));

	// placement new, does nothing
	pLarge = new (pTemp) xtyLargeBlock(Size);
	
	/* Notice: only the first xtyPoolNode's member 'm_pLarge' is in use, others are all null */
	
	m_PoolLock.Lock();
	
	pLarge->m_pNext = m_PoolNode.m_pLarge;
	m_PoolNode.m_pLarge = pLarge;
	
	m_PoolLock.UnLock();
	
	return pLarge->m_pData;
}

void* xtyPool::PoolAllocate(const size_t Size)
{
	xtyPool* pPool = xtyPool::GetInstance();
	
	if(pPool == NULL)
	{
#ifdef DEBUG
		std::cout << "In xtyPool::PoolAllocate(): ";
		std::cout << "Need to call xtyPool::Create() first" << std::endl;
#endif
		exit(XTY_ERROR);
	}
	
	return pPool->DoPoolAllocate(Size);
}

