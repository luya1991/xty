
#ifndef XTY_ALLOC_H
#define XTY_ALLOC_H

class xtyAlloc
{
public:
	xtyAlloc()
	{/* does nothing but need a definition */}
	
	virtual ~xtyAlloc()
	{/* does nothing but need a definition */}

private:
	xtyAlloc(const xtyAlloc&);
	xtyAlloc& operator = (const xtyAlloc&);

public:
	/*
	 * 2015-11-11
	 * because 'Allocate' & 'DeAllocate' need to be impled
	 * as static-member-functions, so they cannot be virtual.
	 * therefore, this interface-class has no explicit interface
	 * definitions.
	 * However, we can still manage to impl interface-consistency
	 * by standardize 'Allocate' & 'DeAllocate' in derived classes
	 * like following:
	 *
	 * 	static void* Allocate(const size_t Size);
	 * 	static void DeAllocate(void* Pointer);
	 */

	/*
	 * 2015-11-17 new thoughts
	 * static funcs can be called in non-static func's range
	 * so, we can still standardize the interface of allocator
	 * class hierachy using virtual fun like the following:
	 */

	virtual void* Allocate(const size_t Size) = 0;
	virtual void DeAllocate(void* Pointer) = 0;
};

#endif

