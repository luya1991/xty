
#ifndef XTY_SHARED_POOL_H
#define XTY_SHARED_POOL_H

#define XTY_PROJ_ID_ARRAY_SIZE 16

typedef struct xtyProjIDArray_t
{
	// multi-process lock
	xtySpinLock m_Lock;
	
	// index of 'proj_id' array
	int m_ProjIDIndex;
	
	// 'proj_id' array for different ipc keys
	int m_ProjIDArray[XTY_PROJ_ID_ARRAY_SIZE];

}xtyProjIDArray;

class xtySharedLargeBlock
{
private:
	explicit xtySharedLargeBlock(const size_t Size, xtyProjIDArray* const pProjIDArray);
	~xtySharedLargeBlock();

private:
	xtySharedLargeBlock(const xtySharedLargeBlock&);
	xtySharedLargeBlock& operator = (const xtySharedLargeBlock&);

public:
	friend class xtySharedPool;

public:
#ifdef DEBUG
	friend class xtySharedPoolTest;
#endif

private:
	int m_LargeSharedMemoryID;
	void* m_pLargeSharedMemory;
	xtySharedLargeBlock* m_pNext;
};

class xtySharedPoolNode
{
private:
	// need to use 'placement-new'
	explicit xtySharedPoolNode(const int SharedMemoryID, void* const SharedMemoryAddr);
	~xtySharedPoolNode();

private:
	xtySharedPoolNode(const xtySharedPoolNode&);
	xtySharedPoolNode& operator = (const xtySharedPoolNode&);
	
private:
	// auxiliary method, performing 4-bytes memory alignment on a pool-allocated object's ptr
	inline char* GetAlignedPtr()
	{
		// reference: (nginx) ngx_palloc.c
		return (char*) (((uintptr_t) (m_pEmptyStart) + 
		    (uintptr_t) (XTY_PTR_ALIGNMENT - 1)) & ~((uintptr_t) (XTY_PTR_ALIGNMENT - 1)));
	}

public:
	friend class xtySharedPool;

public:
#ifdef DEBUG
	friend class xtySharedPoolTest;
#endif

private:
	// shared memory handle
	const int m_SharedMemoryID;
	
	// shared memory base address
	void* const m_pSharedMemory;
	
	// pool managing pointers
	char* m_pEmptyStart;
	char* m_pEmptyEnd;
	xtySharedPoolNode* m_pNext;
	xtySharedLargeBlock* m_pLarge;
};

class xtySharedPool : public xtyAlloc
{
private:
	// need to use 'placement-new'
	explicit xtySharedPool(const int SharedMemoryID, void* const pSharedMemory, xtyProjIDArray* const pProjIDArray);
	virtual ~xtySharedPool();

private:
	xtySharedPool(const xtySharedPool&);
	xtySharedPool& operator = (const xtySharedPool&);

public:
	
	/* 
	 * 2015-07-20
	 * Child process handle-inheritance problem
	 * (Problem description: See 'xty_shm_test.cpp')
	 *
	 * Key point: dynamic allocation ---> static allocation
	 *
	 * Now we set up a shared memory object for ipc usages
	 * rather than managing some shared memory's pointer.
	 *
	 * Although it is not the best solution, it is simple to impl
	 * and does not have other sophisticated problems or bugs.
	 */
	 
	// 2015-08-14
	// use 'obj' as singleton-impl instead of 'ptr'
	// in order to avoid child-inheritance problem
	inline static xtySharedPool* GetInstance() { return m_pSharedPool; }
	
	// allocate a block of memory from shared pool, with 4-bytes alignment
	static void* SharedPoolAllocate(const size_t Size);
	
	// 2015-09-08 added interface-consistency for all allocaters
	virtual void* Allocate(const size_t Size) { return SharedPoolAllocate(Size); }
	
	// 2015-09-08 added interface-consistency for all allocaters
	// does nothing because 'xtySharedPool' has self-clean-up mechanism
	virtual void DeAllocate(void*) { return; }

public:
#ifdef DEBUG
	friend class xtySharedPoolTest;
#endif

public:
	// added on 2015-11-11, for 'xtyFundamentalFacility'
	friend class xtyFundamentalFacility;

private:
	
	/* 2015-11-19 abandoned old-version of 'Create' and 'Destroy' */
	
	/*
	// 2015-11-11 added interface-consistency for all singletons
	inline static void Create()
	{
		// 2015-11-11
		// local-static-object will only be constructed once
		// there might be no need to use 'lock' & 'double-check' stuff
		static xtySharedPool Singleton(XTY_DEFAULT_SHARED_POOL_SIZE);
		m_pSharedPool = &Singleton;
	}
	
	// 2015-11-11 added interface-consistency for all singletons
	inline static void Destroy()
	{
		m_pSharedPool = NULL;
	}
	*/
	
	/* 2015-11-19 apply new-version of 'Create' and 'Destroy' */
	
	// singleton create func
	static void Create();
	
	// singleton destroy func
	static void Destroy();
	
	// auxiliary method for 'Create()', allocating the shared pool with the given 'proj_id'
	static void* SharedMemoryAlloc(const size_t Size, int* const pSharedMemoryID, xtyProjIDArray* const pProjIDArray);
	
	// auxiliary method for 'Destroy()', cleaning up the allocated memory
	static void SharedMemoryCleanUp(void* pNeedToClean);

	// auxiliary method for 'SharedPoolAllocate()', handling actual allocation work
	void* DoSharedPoolAllocate(const size_t Size);
	
	// auxiliary method for 'DoSharedPoolAllocate()', handling large block allocation
	void* DoPoolAllocateLarge(const size_t Size);

private:
	xtySpinLock m_SharedPoolLock;
	xtySharedPoolNode m_SharedPoolNode;
	xtyProjIDArray* m_pSharedProjIDArray;
	
	static xtySpinLock m_SingletonLock;
	static xtySharedPool* m_pSharedPool;
};

#endif

