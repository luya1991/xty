
#ifndef XTY_POOL_H
#define XTY_POOL_H

// for pool-allocated ptr alignment
#define XTY_PTR_ALIGNMENT	sizeof(unsigned long)

// for xtyPoolNode 16-bytes alignment
#define XTY_POOL_ALIGNMENT	16

// reference: (nginx) ngx_palloc.c
#define XTY_LARGE_BLOCK_SIZE	4096
#define XTY_DEFAULT_POOL_SIZE	(4 * 4096)

class xtyLargeBlock
{
private:
	explicit xtyLargeBlock(const size_t);
	~xtyLargeBlock();

private:
	xtyLargeBlock(const xtyLargeBlock&);
	xtyLargeBlock& operator = (const xtyLargeBlock&);

public:
	friend class xtyPool;

public:
#ifdef DEBUG
	friend class xtyPoolTest;
#endif

private:
	void* m_pData;
	xtyLargeBlock* m_pNext;
};

class xtyPoolNode
{
private:
	// need to use 'placement-new'
	explicit xtyPoolNode(void* const pPoolAddr);
	~xtyPoolNode();

private:
	xtyPoolNode(const xtyPoolNode&);
	xtyPoolNode& operator = (const xtyPoolNode&);
	
private:
	// auxiliary method, performing 4-bytes memory alignment on a pool-allocated object's ptr
	inline char* GetAlignedPtr()
	{
		// reference: (nginx) ngx_palloc.c
		return (char*) (((uintptr_t) (m_pEmptyStart) + 
		    (uintptr_t) (XTY_PTR_ALIGNMENT - 1)) & ~((uintptr_t) (XTY_PTR_ALIGNMENT - 1)));
	}

public:
	friend class xtyPool;

public:
#ifdef DEBUG
	friend class xtyPoolTest;
#endif

private:
	// normal size block managing pointers (size <= 4096 - node - lock)
	char* m_pEmptyStart;
	char* m_pEmptyEnd;
	xtyPoolNode* m_pNext;
	
	// large size block managing pointer (size > 4096 - node - lock)
	xtyLargeBlock* m_pLarge;
	
	/* Notice: only the first xtyPoolNode's member 'm_pLarge' is in use, others are all null */
};

class xtyPool : public xtyAlloc
{
private:
	// need to use 'placement-new'
	explicit xtyPool(void* const pPoolAddr);
	virtual ~xtyPool();

private:
	xtyPool(const xtyPool&);
	xtyPool& operator = (const xtyPool&);
	
public:
	// get singleton pointer
	inline static xtyPool* GetInstance() { return m_pPool; }

	// allocate a block of memory from pool, with 4-bytes alignment
	static void* PoolAllocate(const size_t Size);
	
	// 2015-09-08 added interface-consistency for all allocaters
	virtual void* Allocate(const size_t Size) { return PoolAllocate(Size); }
	
	// 2015-09-08 added interface-consistency for all allocaters
	// does nothing because 'xtyPool' has self-clean-up mechanism
	virtual void DeAllocate(void*) { return; }

public:
#ifdef DEBUG
	friend class xtyPoolTest;
#endif

public:
	// added on 2015-11-11, for 'xtyFundamentalFacility'
	friend class xtyFundamentalFacility;

private:
	// singleton create func
	static void Create();
	
	// singleton destroy func
	static void Destroy();
	
	// auxiliary method for 'Create()', allocating 16-bytes-aligned pool of memory
	static void* MemoryAlignAlloc(const size_t Alignment = XTY_POOL_ALIGNMENT);
	
	// auxiliary method for 'Destroy()', cleaning up the allocated memory
	static void MemoryCleanUp(void* pNeedToClean);
	
	// auxiliary method for 'PoolAllocate()', handling actual allocation work
	void* DoPoolAllocate(const size_t Size);
	
	// auxiliary method for 'DoPoolAllocate()', handling large block allocation
	void* DoPoolAllocateLarge(const size_t Size);

public: 
	/*
	 * 2015-06-11
	 * overloading operator 'new' and 'delete' for customized malloc and free
	 * 
	 * notice:
	 * a. 'new' and 'delete' will always be 'static' even if there is no keyword
	 * 'static' given.
	 *
	 * b. xtyPool::operator new cannot take 'PoolSize' as its parameter, and 
	 * the parameter 'const size_t size' must be 'sizeof(xtyPool)', thus the
	 * following expression does not have the expected behavior:
	 *
	 * 	new xtyPool(PoolSize);
	 *
	 * with the expected behavior being:
	 * (1) alloc 'PoolSize' bytes with 16-bytes alignment. 
	 * (2) call xtyPool's constructor.
	 *   
	 * so, I make operator new do nothing, and let MemoryAlignAlloc() do the actual
	 * allocation work.
	 */
	
	// 2015-06-29 
	// abandoned operator 'new' and 'delete' overloading...
	// because 'placement-new' and 'directly-deconstruction' have the behavior I wanted
	
	// 'new' must take 'size_t' as its first parameter, and must return 'void*'
	// static void* operator new(const size_t size, void* ptr) { return ptr; }
	
	// 'delete' must take 'void*' as its first parameter, and must return 'void'
	// static void operator delete(void* ptr) { /* does nothing, just to avoid double-free */}
	
private:
	xtyMutex m_PoolLock;
	xtyPoolNode m_PoolNode;
	
	static xtySpinLock m_SingletonLock;
	static xtyPool* m_pPool;
};

#endif

