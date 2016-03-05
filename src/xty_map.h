
#ifndef XTY_MAP_H
#define XTY_MAP_H

template<typename Key, typename Value>
class xtyMapNode
{
public:
	// for sentinel node
	xtyMapNode<Key, Value>()
	: m_Key(Key()), m_Value(Value()), First(m_Key), Second(m_Value)
	{/* does nothing but need a definition */}

	// for key-value node
	explicit xtyMapNode<Key, Value>(const Key& KeyPara, const Value& ValuePara)
	: m_Key(KeyPara), m_Value(ValuePara), First(m_Key), Second(m_Value)
	{/* does nothing but need a definition */}
	
	// for key-search node
	explicit xtyMapNode<Key, Value>(const Key& KeyPara)
	: m_Key(KeyPara), m_Value(Value()), First(m_Key), Second(m_Value)
	{/* does nothing but need a definition */}
	
	~xtyMapNode<Key, Value>()
	{/* does nothing but need a definition */}

public:
	// allow copy-constructor
	xtyMapNode<Key, Value>(const xtyMapNode<Key, Value>& Node)
	: m_Key(Node.m_Key), m_Value(Node.m_Value), First(m_Key), Second(m_Value)
	{
		// copy-constructor also has member-init list
		
		/*
		m_Key = Node.m_Key;
		m_Value = Node.m_Value;
		First = m_Key;
		Second = m_Value;
		*/
	}
	
	// allow copy-assignment
	xtyMapNode<Key, Value>& operator = (const xtyMapNode<Key, Value>& Node)
	{
		// copy-assignment does not have member-copy list
		
		m_Key = Node.m_Key;
		m_Value = Node.m_Value;
		First = m_Key;
		Second = m_Value;
		
		return *this;
	}

public:
	// comparison-operator overloading
	inline bool operator > (const xtyMapNode& Node) const { return (m_Key > Node.m_Key); }
	inline bool operator < (const xtyMapNode& Node) const { return (m_Key < Node.m_Key); }
	inline bool operator == (const xtyMapNode& Node) const { return (m_Key == Node.m_Key); }

	// output-operator overloading
	inline friend std::ostream& operator << (std::ostream& out, const xtyMapNode<Key, Value>& node)
	{
		out << "node-key: " << node.m_Key << std::endl;
		out << "node-value: " << node.m_Value;
		return out;
	}
	
public:
	// 2015-09-14 problem
	// whether 'return-reference_type' property will be
	// damaged by 'return-value_type' in the middle?
	// 2015-09-15 solved
	// yes, it will be damaged because of object copying,
	// so the whole return-chain needs to be 'return-reference_type'
	
	// 2015-09-15 p.m. get-value_type: const, get-ptr(ref)_type: non-const(because of possible access later)
	inline Value GetValue() const { return m_Value; }
	inline Value* GetValuePtr() { return &m_Value; }
	
public:
	// 2015-09-08 haven't impled yet
	// generate key for 'xtyRbTree' node-sorting
	//
	// 2015-09-16 abandoned
	// cannot guarantee the uniqueness of the key
	//
	// template<typename __Key, typename __Value>
	// friend inline int GenerateKeyForRbTree(const xtyMapNode<__Key, __Value>&);
	
	template<typename __Key, typename __Value> friend class xtyMap;
	template<typename __Key, typename __Value> friend class xtyMapIterator;
	
private:
	Key m_Key;
	Value m_Value;

public:
	const Key& First;
	const Value& Second;
};

// 2015-09-07
// problem: function-template explicit-specialization causes 'conflict defination'
// solved: keyword 'inline' will not create any external symbols, it simply expands
// the code-segment each place where the function-template is called.

// 2015-09-16 abandoned
// cannot guarantee the uniqueness of the key
//
// template<typename __Key, typename __Value>
// inline int GenerateKeyForRbTree(const xtyMapNode<__Key, __Value>& Node)
// {
//	return GenerateKeyForRbTree(Node.m_Key);
// }

template<typename Key, typename Value>
class xtyMap
{
public:
	explicit xtyMap<Key, Value>(xtyAlloc* pAlloc);
	~xtyMap<Key, Value>();

private:
	xtyMap<Key, Value>(const xtyMap<Key, Value>&);
	xtyMap<Key, Value>& operator = (const xtyMap<Key, Value>&);

public:
	// get map's size
	inline size_t Size() const { return m_RbTree.Size(); }
	
	// operator '[]' overloading
	Value& operator [] (const Key&);
	
	// basic insert methods for map
	xtyMapNode<Key, Value>* Insert(const Key& KeyPara);
	xtyMapNode<Key, Value>* Insert(const xtyMapNode<Key, Value>& Node);
	xtyMapNode<Key, Value>* Insert(const Key& KeyPara, const Value& ValuePara);
	
	// basic search methods for map
	xtyMapNode<Key, Value>* Search(const Key& KeyPara);
	xtyMapNode<Key, Value>* Search(const xtyMapNode<Key, Value>& Node);
	xtyMapNode<Key, Value>* Search(const Key& KeyPara, const Value& ValuePara);
	
	// basic delete methods for map
	// 2016-02-22, changed return value from 'xtyMapNode*' to 'void'
	void Delete(const Key& KeyPara);
	void Delete(const xtyMapNode<Key, Value>& Node);
	void Delete(const Key& KeyPara, const Value& ValuePara);
	
	// visit the map
	void Visit();
	
	// clear the map
	void Clear();

public:
	template<typename __Key, typename __Value> friend class xtyMapIterator;

public:
#ifdef DEBUG	
	friend class xtyMapTest;
#endif

private:
	// use rbtree as data container to impl map
	xtyRbTree< xtyMapNode<Key, Value> > m_RbTree;
};

template<typename Key, typename Value>
xtyMap<Key, Value>::xtyMap(xtyAlloc* pAlloc) : m_RbTree(pAlloc)
{
#ifdef DEBUG
	std::cout << "In xtyMap::xtyMap()" << std::endl;
#endif
}

template<typename Key, typename Value>
xtyMap<Key, Value>::~xtyMap()
{
#ifdef DEBUG
	std::cout << "In xtyMap::~xtyMap()" << std::endl;
#endif
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Insert(const Key& KeyPara)
{
	xtyRbTreeNode< xtyMapNode<Key, Value> >* pRbtNode = NULL;
	pRbtNode = m_RbTree.Insert(xtyMapNode<Key, Value>(KeyPara));
	return (pRbtNode ? pRbtNode->GetDataPtr() : NULL);
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Insert(const xtyMapNode<Key, Value>& Node)
{
	xtyMapNode<Key, Value>* pInsert = Insert(Node.m_Key);
	
	if (pInsert)
	{
		pInsert->m_Value = Node.m_Value;
		return pInsert;
	}

	return NULL;
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Insert(const Key& KeyPara, const Value& ValuePara)
{
	return Insert(xtyMapNode<Key, Value>(KeyPara, ValuePara));
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Search(const Key& KeyPara)
{
	xtyRbTreeNode< xtyMapNode<Key, Value> >* pRbtNode = NULL;
	pRbtNode = m_RbTree.Search(xtyMapNode<Key, Value>(KeyPara));
	return (pRbtNode ? pRbtNode->GetDataPtr() : NULL);
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Search(const xtyMapNode<Key, Value>& Node)
{
	xtyMapNode<Key, Value>* pFound = Search(Node.m_Key);
	
	if (pFound)
	{
		// both key and value are the same
		if (Node.m_Value == pFound->m_Value)
		{
			std::cout << "In xtyMap::Search(): key and value both match" << std::endl;
			return pFound;
		}
		
		// key matches, but value does not match
		else
		{
			std::cout << "In xtyMap::Search(): key matches, but value does not match" << std::endl;
			return NULL;
		}
	}
	
	return NULL;
}

template<typename Key, typename Value>
xtyMapNode<Key, Value>* xtyMap<Key, Value>::Search(const Key& KeyPara, const Value& ValuePara)
{
	return Search(xtyMapNode<Key, Value>(KeyPara, ValuePara));
}

template<typename Key, typename Value>
void xtyMap<Key, Value>::Delete(const Key& KeyPara)
{
	// only consider key, ignore value
	m_RbTree.Delete(xtyMapNode<Key, Value>(KeyPara));
}

template<typename Key, typename Value>
void xtyMap<Key, Value>::Delete(const xtyMapNode<Key, Value>& Node)
{
	// consider both key and value
	xtyMapNode<Key, Value>* pDelete = Search(Node);
	
	if (pDelete)
		Delete(Node.m_Key);
}

template<typename Key, typename Value>
void xtyMap<Key, Value>::Delete(const Key& KeyPara, const Value& ValuePara)
{
	// consider both key and value
	Delete(xtyMapNode<Key, Value>(KeyPara, ValuePara));
}

template<typename Key, typename Value>
Value& xtyMap<Key, Value>::operator [] (const Key& KeyPara)
{
	xtyMapNode<Key, Value>* pFound = Search(KeyPara);
	
	if (pFound)
		// return reference-type by '*ptr'
		return *(pFound->GetValuePtr());
	
	else
	{
		// 'KeyPara' is not in the map yet
		xtyMapNode<Key, Value>* pNode = Insert(KeyPara);

		// return reference-type by '*ptr'
		return *(pNode->GetValuePtr());
	}
}

template<typename Key, typename Value>
void xtyMap<Key, Value>::Visit()
{
	m_RbTree.NonRecursiveVisit();
}

template<typename Key, typename Value>
void xtyMap<Key, Value>::Clear()
{
	m_RbTree.Clear();
}

template<typename Key, typename Value>
class xtyMapIterator
{
private:
	// Attention: this method is only used inside of class by member 'Begin()' & 'End()'
	explicit xtyMapIterator(const xtyRbTreeIterator< xtyMapNode<Key, Value> >& Iter, xtyMap<Key, Value>* const pRelatedMap)
	: m_RbTreeIter(Iter), m_pRelatedMap((pRelatedMap == NULL ? throw XTY_NULL_POINTER : pRelatedMap))
	{
		// this method has to guarantee that 'Iter' is in 'pRelatedMap'
	}
	
public:
	explicit xtyMapIterator(xtyMap<Key, Value>* const pRelatedMap)
	: m_RbTreeIter((pRelatedMap == NULL ? throw XTY_NULL_POINTER : &(pRelatedMap->m_RbTree))),
	m_pRelatedMap((pRelatedMap == NULL ? throw XTY_NULL_POINTER : pRelatedMap))
	{

	}
	
	~xtyMapIterator()
	{
	
	}

public:
	xtyMapIterator(const xtyMapIterator& Iter)
	: m_RbTreeIter(Iter.m_RbTreeIter), m_pRelatedMap(Iter.m_pRelatedMap)
	{
		// use init-list for copy-construction
	}
	
	xtyMapIterator& operator = (const xtyMapIterator& Iter)
	{
		m_RbTreeIter = Iter.m_RbTreeIter;
		m_pRelatedMap = Iter.m_pRelatedMap;
		return *this;
	}
public:
	xtyMapNode<Key, Value>* operator -> () { return m_RbTreeIter.m_pNode->GetDataPtr(); }
	xtyMapNode<Key, Value>& operator * () { return *(m_RbTreeIter.m_pNode->GetDataPtr()); }
	
public:
	// ok to return 'ref_type'
	xtyMapIterator& operator ++ ()
	{
		++m_RbTreeIter;
		return *this;
	}
	
	// cannot return 'ref_type' because 'tmp' will destruct
	xtyMapIterator operator ++ (int)
	{
		xtyMapIterator tmp = *this;
		++m_RbTreeIter;
		return tmp;
	}
	
	// ok to return 'ref_type'
	xtyMapIterator& operator -- ()
	{
		--m_RbTreeIter;
		return *this;
	}
	
	// cannot return 'ref_type' because 'tmp' will destruct
	xtyMapIterator operator -- (int)
	{
		xtyMapIterator tmp = *this;
		--m_RbTreeIter;
		return tmp;
	}
	
	bool operator == (const xtyMapIterator& Iter)
	{
		return ((m_RbTreeIter == Iter.m_RbTreeIter) && (m_pRelatedMap == Iter.m_pRelatedMap));
	}
	
	bool operator != (const xtyMapIterator& Iter)
	{
		return ((m_RbTreeIter != Iter.m_RbTreeIter) || (m_pRelatedMap != Iter.m_pRelatedMap));
	}
	
public:
	xtyMapIterator Begin() const
	{
		// calling the private constructor
		return xtyMapIterator(m_RbTreeIter.Begin(), m_pRelatedMap);
	}
	
	xtyMapIterator ReverseBegin() const
	{
		// calling the private constructor
		return xtyMapIterator(m_RbTreeIter.ReverseBegin(), m_pRelatedMap);
	}
	
	xtyMapIterator End() const
	{
		// calling the private constructor
		return xtyMapIterator(m_RbTreeIter.End(), m_pRelatedMap);
	}
	
private:
	xtyRbTreeIterator< xtyMapNode<Key, Value> > m_RbTreeIter;
	xtyMap<Key, Value>* m_pRelatedMap;
};

#endif

