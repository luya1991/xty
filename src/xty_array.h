
#ifndef XTY_ARRAY_H
#define XTY_ARRAY_H

#define XTY_DEFAULT_ARRAY_SIZE 4

template<typename T>
inline void __Construct(T* Position, const T& Value)
{
	// call ctor(reference: stl_construct.h)
	new (static_cast<void*>(Position)) T(Value);
}

template<typename T>
inline void __Destruct(T* Pointer)
{
	// call dtor(reference: stl_construct.h)
	Pointer->~T();
}

template<typename T>
class xtyArray
{
public:
	xtyArray(xtyAlloc* const pAlloc);
	~xtyArray();

public:
	// allow copying behavior
	xtyArray(const xtyArray<T>&);
	xtyArray& operator = (const xtyArray<T>&);

private:
	inline bool operator == (const xtyArray& Array) const
	{
		if ((m_NodeCount == Array.m_NodeCount)
		&& (m_NodeCapacity == Array.m_NodeCapacity)
		&& (m_pDataStorage == Array.m_pDataStorage)
		&& (m_pAlloc == Array.m_pAlloc))
			return true;
		
		return false;
	}

public:
	inline T& operator [] (const size_t pos)
	{
		if (pos >= m_NodeCount)
			throw XTY_INVALID_ARRAY_INDEX;
		
		return m_pDataStorage[pos];
	}

public:
	inline unsigned int Size() const { return m_NodeCount; }
	inline unsigned int GetNodeCount() const { return m_NodeCount; }
	
	inline T& Front() { return m_pDataStorage[0]; }
	inline T& Back() { return m_pDataStorage[m_NodeCount - 1]; }

public:
	void PushBack(const T& PushValue);
	void PopBack();
	void Clear();
	void Visit();

public:
#ifdef DEBUG
	friend class xtyArrayTest;
#endif

private:
	size_t m_NodeCount;
	size_t m_NodeCapacity;
	T* m_pDataStorage;
	
	// added on 2016-02-21
	xtyAlloc* m_pAlloc;
};

template<typename T>
xtyArray<T>::xtyArray(xtyAlloc* const pAlloc)
: m_NodeCount(0), m_NodeCapacity(XTY_DEFAULT_ARRAY_SIZE)
{
#ifdef DEBUG
	std::cout << "In xtyArray::xtyArray()" << std::endl;
#endif	
	if (pAlloc == NULL)
		xtyError::ErrorExit("Invalid initialization: Null xtyAlloc*");
	
	m_pAlloc = pAlloc;
	
	m_pDataStorage = (T*) m_pAlloc->Allocate(m_NodeCapacity * sizeof(T));
}

template<typename T>
xtyArray<T>::~xtyArray()
{
#ifdef DEBUG
	std::cout << "In xtyArray::~xtyArray()" << std::endl;
#endif
	if (m_pDataStorage)
	{
		for (size_t i = 0; i < m_NodeCount; i++)
			__Destruct(&m_pDataStorage[i]);
	}
}

template<typename T>
xtyArray<T>::xtyArray(const xtyArray<T>& Array)
: m_NodeCount(Array.m_NodeCount), m_NodeCapacity(Array.m_NodeCapacity), m_pAlloc(Array.m_pAlloc)
{
#ifdef DEBUG
	std::cout << "In xtyArray::xtyArray(const xtyArray&)" << std::endl;
#endif
	m_pDataStorage = (T*) m_pAlloc->Allocate(m_NodeCapacity * sizeof(T));
	
	// 2016-02-19, this is shallow-copy, we need deep-copy
	// memcpy(m_pDataStorage, Array.m_pDataStorage, m_NodeCount * sizeof(T));
	
	// 2016-02-19, this is deep-copy, copy details should be impled in T's copy-ctor
	for (size_t i = 0; i < m_NodeCount; i++)
		__Construct(&m_pDataStorage[i], Array.m_pDataStorage[i]);
}

template<typename T>
xtyArray<T>& xtyArray<T>::operator = (const xtyArray<T>& Array)
{
#ifdef DEBUG
	std::cout << "In xtyArray::operator = (const xtyArray&)" << std::endl;
#endif
	// refer to private method 'opt=='
	if (*this == Array)
		return *this;
	
	m_NodeCount = Array.m_NodeCount;
	m_NodeCapacity = Array.m_NodeCapacity;
	m_pAlloc = Array.m_pAlloc;
	
	m_pDataStorage = (T*) m_pAlloc->Allocate(m_NodeCapacity * sizeof(T));
	
	// 2016-02-19, this is shallow-copy, we need deep copy
	// memcpy(m_pDataStorage, Array.m_pDataStorage, m_NodeCount * sizeof(T));
	
	// 2016-02-19, this is deep-copy, copy details should be impled in T's copy-ctor
	for (size_t i = 0; i < m_NodeCount; i++)
		__Construct(&m_pDataStorage[i], Array.m_pDataStorage[i]);
	
	return *this;
}

template<typename T>
void xtyArray<T>::PushBack(const T& PushValue)
{
#ifdef DEBUG
	std::cout << "In xtyArray::PushBack()" << std::endl;
#endif
	// still room for new element, just call ctor
	if (m_NodeCapacity > m_NodeCount)
		__Construct(&m_pDataStorage[m_NodeCount], PushValue);
	
	// no room for new element, need to realloc
	else
	{
		// double the size of current array
		m_NodeCapacity *= 2;
		
		// mark down the old data storage ptr
		T* pOldDataStorage = m_pDataStorage;
		
		m_pDataStorage = (T*) m_pAlloc->Allocate(m_NodeCapacity * sizeof(T));
		
		// 2016-02-19, this is shallow-copy, we need deep copy
		// memcpy(m_pDataStorage, pOldDataStorage, m_NodeCount * sizeof(T));
		
		// 2016-02-19, this is deep-copy, copy details should be impled in T's copy-ctor
		for (size_t i = 0; i < m_NodeCount; i++)
			__Construct(&m_pDataStorage[i], pOldDataStorage[i]);
		
		// call ctor to push 'PushValue' on the back
		__Construct(&m_pDataStorage[m_NodeCount], PushValue);
		
		// call dtor to clean up old stuff
		for (size_t i = 0; i < m_NodeCount; i++)
			__Destruct(&pOldDataStorage[i]);
	}
	
	m_NodeCount++;
}

template<typename T>
void xtyArray<T>::PopBack()
{
#ifdef DEBUG
	std::cout << "In xtyArray::PopBack()" << std::endl;
#endif
	// call dtor to pop out an element on the back
	__Destruct(&m_pDataStorage[m_NodeCount - 1]);
	
	m_NodeCount--;
}

template<typename T>
void xtyArray<T>::Clear()
{
#ifdef DEBUG
	std::cout << "In xtyArray::Clear()" << std::endl;
#endif
	for (size_t i = 0; i < m_NodeCount; i++)
		__Destruct(&m_pDataStorage[i]);
	
	m_NodeCount = 0;
}

template<typename T>
void xtyArray<T>::Visit()
{
#ifdef DEBUG
	std::cout << "In xtyArray::Visit()" << std::endl;
#endif
	std::cout << "count: " << m_NodeCount << ", capacity: " << m_NodeCapacity << std::endl;
	
	for (size_t i = 0; i < m_NodeCount; i++)
		std::cout << m_pDataStorage[i] << " ";
	
	std::cout << std::endl;
}

#endif

