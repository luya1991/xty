
#ifndef XTY_QUEUE_H
#define XTY_QUEUE_H

template<typename T>
class xtyQueueNode
{
private:
	// for sentinel node
	xtyQueueNode<T>();

public:	
	// for normal nodes
	explicit xtyQueueNode<T>(const T& Data);
	
	// deconstructor
	~xtyQueueNode<T>();

private:
	xtyQueueNode<T>(const xtyQueueNode<T>&);
	xtyQueueNode<T>& operator = (const xtyQueueNode<T>&);

public:
	// 2015-09-15 p.m. get-value_type: const, get-ptr(ref)_type: non-const(because of possible access later)
	inline T GetData() const { return m_Data; }
	inline T* GetDataPtr() { return &m_Data; }
	
	inline xtyQueueNode<T>* GetPrev() { return m_pPrev; }
	inline xtyQueueNode<T>* GetNext() { return m_pNext; }
	
public:
	// 'typename U': template param cannot be 'T' again, name-shading
	template<typename U> friend class xtyQueue;

private:
	T m_Data;
	xtyQueueNode<T>* m_pPrev;
	xtyQueueNode<T>* m_pNext;
};

template<typename T>
class xtyQueue
{
public:
	// added on 2015-11-17, inherits 'xtyAlloc*' from outside
	explicit xtyQueue<T>(xtyAlloc* pAlloc);
	~xtyQueue<T>();

private:
	xtyQueue<T>(const xtyQueue<T>&);
	xtyQueue<T>& operator = (const xtyQueue<T>&);

public:
	inline unsigned int Size() const { return m_NodeCount; }
	inline unsigned int GetNodeCount() const { return m_NodeCount; }
	
	inline void Clear()
	{
#ifdef DEBUG
		std::cout << "In xtyQueue::Clear()" << std::endl;
#endif
		xtyQueueNode<T>* p = m_Sentinel.m_pNext;
		xtyQueueNode<T>* q = p;
	
		while (p != &m_Sentinel)
		{
			q = p;
			p = p->m_pNext;
			
			// 2016-02-22, add dtor to avoid mem-leak
			q->~xtyQueueNode<T>();
		}
	
		m_Sentinel.m_pPrev = &m_Sentinel;
		m_Sentinel.m_pNext = &m_Sentinel;
		
		m_NodeCount = 0;
	}
	
	// enqueue a node at back, non-throw
	void EnQueue(const T&);
	
	// dequeue a node at front, possibly-throw
	T DeQueue(void);

	// show the whole queue
	void Visit();
	
	// 2015-09-16 newly added
	xtyQueueNode<T>* Search(const T&);

public:
#ifdef DEBUG
	friend class xtyQueueTest;
#endif
			
private:
	unsigned int m_NodeCount;
	xtyQueueNode<T> m_Sentinel;
	
	// added on 2015-11-17
	xtyAlloc* m_pAlloc;
};

template<typename T>
xtyQueueNode<T>::xtyQueueNode() : m_pPrev(this), m_pNext(this)
{
#ifdef DEBUG
	// std::cout << "In xtyQueueNode::xtyQueueNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyQueueNode<T>::xtyQueueNode(const T& Data) : m_Data(Data), m_pPrev(NULL), m_pNext(NULL)
{
#ifdef DEBUG
	// std::cout << "In xtyQueueNode::xtyQueueNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyQueueNode<T>::~xtyQueueNode()
{
#ifdef DEBUG
	// std::cout << "In xtyQueueNode::~xtyQueueNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyQueue<T>::xtyQueue(xtyAlloc* pAlloc) : m_NodeCount(0), m_Sentinel(xtyQueueNode<T>())
{
#ifdef DEBUG
	std::cout << "In xtyQueue::xtyQueue()" << std::endl;
#endif
	if (pAlloc == NULL)
		xtyError::ErrorExit("Invalid initialization: Null xtyAlloc*");
	
	m_pAlloc = pAlloc;
}

template<typename T>
xtyQueue<T>::~xtyQueue()
{
#ifdef DEBUG
	std::cout << "In xtyQueue::~xtyQueue()" << std::endl;
#endif	
	xtyQueueNode<T>* p = m_Sentinel.m_pNext;
	xtyQueueNode<T>* q = p;

	while (p != &m_Sentinel)
	{
		q = p;
		p = p->m_pNext;
		
		// 2016-02-22, add dtor to avoid mem-leak
		q->~xtyQueueNode<T>();
	}
}

template<typename T>
void xtyQueue<T>::EnQueue(const T& EnQueueValue)
{
#ifdef DEBUG
	std::cout << "In xtyQueue::EnQueue()" << std::endl;
#endif
	xtyQueueNode<T>* pEnQueueNode = (xtyQueueNode<T>*) m_pAlloc->Allocate(sizeof(xtyQueueNode<T>));
	pEnQueueNode = new (pEnQueueNode) xtyQueueNode<T>(EnQueueValue);
	
	pEnQueueNode->m_pPrev = m_Sentinel.m_pPrev;
	pEnQueueNode->m_pNext = &m_Sentinel;
	m_Sentinel.m_pPrev->m_pNext = pEnQueueNode;
	m_Sentinel.m_pPrev = pEnQueueNode;
	
	m_NodeCount++;
}

template<typename T>
T xtyQueue<T>::DeQueue()
{
#ifdef DEBUG
	std::cout << "In xtyQueue::Dequeue()" << std::endl;
#endif
	if (m_NodeCount == 0)
		throw xtyException(XTY_EMPTY_QUEUE);
	
	xtyQueueNode<T>* pDeQueueNode = m_Sentinel.m_pNext;
	
	m_Sentinel.m_pNext = pDeQueueNode->m_pNext;
	pDeQueueNode->m_pNext->m_pPrev = &m_Sentinel;
	pDeQueueNode->m_pPrev = NULL;
	pDeQueueNode->m_pNext = NULL;
	
	// mark down dequeue value before dtor
	T DeQueueValue = pDeQueueNode->m_Data;
	
	// 2016-02-22, add dtor to avoid mem-leak
	pDeQueueNode->~xtyQueueNode();
	
	m_NodeCount--;
	
	return DeQueueValue;
}

template<typename T>
void xtyQueue<T>::Visit()
{
#ifdef DEBUG
	std::cout << "In xtyQueue::Visit()" << std::endl;
#endif	
	if (m_NodeCount == 0)
		std::cout << "Empty queue, nothing to visit" << std::endl; 
	
	else
	{
		std::cout << "Node count: " << m_NodeCount << std::endl;
		
		xtyQueueNode<T>* p = m_Sentinel.m_pNext;
		
		while (p != &m_Sentinel)
		{
			std::cout << "p.data: " << p->m_Data << std::endl;
			p = p->m_pNext;
		}
	}
}

template<typename T>
xtyQueueNode<T>* xtyQueue<T>::Search(const T& SearchValue)
{
#ifdef DEBUG
	// std::cout << "In xtyQueue::Search()" << std::endl;
#endif	
	xtyQueueNode<T>* pFound = NULL;
	
	xtyQueueNode<T>* p = m_Sentinel.m_pNext;
	
	while (p != &m_Sentinel)
	{
		if (p->m_Data == SearchValue)
		{
			pFound = p;
			break;
		}
		
		p = p->m_pNext;
	}
	
	return pFound;
}

#endif

