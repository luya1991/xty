
#ifndef XTY_RBTREE_H
#define XTY_RBTREE_H

#define XTY_RBT_RED	0
#define XTY_RBT_BLACK	1

// 2015-09-07
// problem: function-template explicit specialization causes 'conflict defination'
// solved: keyword 'inline' will not create any external symbols, it simply expands
// the code-segment each place where the function-template is called.
 
/*
template<typename Key>
inline int GenerateKeyForRbTree(const Key& KeyPara)
{
	// generic
	return KeyPara.ToRbTreeKey();
}

template<>
inline int GenerateKeyForRbTree(const int& KeyPara)
{
	// specialized
	return KeyPara;
}
*/

/*
 * 2015-09-14 abandoned
 * problem with specialized version for 'std::string':
 * because it cannot guarantee that the key will be different
 * for different input strings, for example:
 * 	string1: (1 * 10 + 2 ) * 10 = 120
 *	string2: 12 * 10 = 120
 * string1 is not equal to string2, but their keys are the same.
 * therefore, this 'GenerateKeyForRbTree' thinking is not correct.
 */

/* 
template<>
inline int GenerateKeyForRbTree(const std::string& KeyPara)
{
	// specialized
	int res = 0;
	
	size_t i;
	for (i = 0; i < KeyPara.size(); i++)
	{
		res += static_cast<int>(KeyPara[i]);
		res *= 10;
	}
	
	return res;
}
*/

template<typename T>
class xtyRbTreeNode
{
private:
	// for sentinel node
	xtyRbTreeNode<T>();

public:	
	// for normal nodes
	explicit xtyRbTreeNode<T>(const T& Data);
	
	// deconstructor
	~xtyRbTreeNode<T>();

private:
	xtyRbTreeNode<T>(const xtyRbTreeNode<T>&);
	xtyRbTreeNode& operator = (const xtyRbTreeNode<T>&);

public:
	// 2015-09-15 p.m. get-value_type: const, get-ptr(ref)_type: non-const(because of possible access later)
	inline T GetData() const { return m_Data; }
	inline T* GetDataPtr() { return &m_Data; }
	
	inline xtyRbTreeNode<T>* GetParent() { return m_pParent; }
	inline xtyRbTreeNode<T>* GetLeft() { return m_pLeft; }
	inline xtyRbTreeNode<T>* GetRight() { return m_pRight; }

public:
	// 'typename U': template param cannot be 'T' again, name-shading
	template<typename U> friend class xtyRbTree;
	
	// added on 2016-01-24
	template<typename U> friend class xtyRbTreeIterator;
	
	// added on 2016-01-31
	template<typename __Key, typename __Value> friend class xtyMapIterator;
	
private:
	
	/*
	 * 2015-09-16
	 * big mofification, abandoned 'int m_Key'
	 * because it is impossible to impl a 'GenerateKey()' function for any 'string-int' map
	 * obviously, there are countless strings, while the max value of int is only '4294967295'
	 * according to the 'drawer principle', there must be different strings mapping to the same
	 * int value. 
	 *
	 * 2015-09-16 abandoned
	 * int m_Key;
	 */
	 
	T m_Data;
	xtyRbTreeNode<T>* m_pParent;
	xtyRbTreeNode<T>* m_pLeft;
	xtyRbTreeNode<T>* m_pRight;
	unsigned int m_Color;
};

template<typename T>
class xtyRbTree
{
public:
	// added on 2015-11-17, inherits 'xtyAlloc*' from outside
	explicit xtyRbTree<T>(xtyAlloc* pAlloc);
	~xtyRbTree<T>();
	
private:
	xtyRbTree<T>(const xtyRbTree<T>&);
	xtyRbTree<T>& operator = (const xtyRbTree<T>&);

private:	
	inline xtyRbTreeNode<T>* LeftMost(xtyRbTreeNode<T>* pNode)
	{
		while (pNode->m_pLeft != &m_Sentinel)
			pNode = pNode->m_pLeft;
		
		return pNode;
	}
	
	inline xtyRbTreeNode<T>* RightMost(xtyRbTreeNode<T>* pNode)
	{
		while (pNode->m_pRight != &m_Sentinel)
			pNode = pNode->m_pRight;
		
		return pNode;
	}
	
	inline void TransPlant(xtyRbTreeNode<T>* pOld, xtyRbTreeNode<T>* pNew)
	{
		if (pOld->m_pParent == &m_Sentinel)
			m_pRoot = pNew;
		
		else if (pOld == pOld->m_pParent->m_pLeft)
			pOld->m_pParent->m_pLeft = pNew;
		
		else
			pOld->m_pParent->m_pRight = pNew;

		/*
		 * 2015-07-19
		 * this operation may cause sentinel to have a non-sentinel 'm_pParent' ptr!
		 * It is important for 'DeleteFixUp()' to locate the fix-up position, especially
		 * when 'pNew' is sentinel node: it can guarantee that pNew's parent ptr is
		 * non-sentinel(pNew's 'm_pParent' ptr will tell the fix-up position).
		 */

		pNew->m_pParent = pOld->m_pParent;
	}
	
	// 2016-02-22, clean up the whole tree recursively
	inline void __Clear_Aux(xtyRbTreeNode<T>* pNode)
	{
		if (pNode == &m_Sentinel)
			return;
		
		if (pNode->m_pLeft != &m_Sentinel)
			__Clear_Aux(pNode->m_pLeft);
		
		if (pNode->m_pRight != &m_Sentinel)
			__Clear_Aux(pNode->m_pRight);
		
		// 2016-02-22, add dtor to avoid mem-leak
		pNode->~xtyRbTreeNode();
	}
	
	void LeftRotate(xtyRbTreeNode<T>*);
	void RightRotate(xtyRbTreeNode<T>*);
	
	void InsertFixUp(xtyRbTreeNode<T>*);
	void DeleteFixUp(xtyRbTreeNode<T>*);
	
public:
	inline xtyRbTreeNode<T>* GetRoot() { return m_pRoot; }
	inline unsigned int Size() const { return m_NodeCount; }
	inline unsigned int GetNodeCount() const { return m_NodeCount; }
	
	inline void Clear()
	{
#ifdef DEBUG
		std::cout << "In xtyRbTree::Clear()" << std::endl;
#endif
		// 2016-02-22, add dtor to avoid mem-leak
		__Clear_Aux(m_pRoot);
		
		m_Sentinel.m_pParent = &m_Sentinel;
		m_Sentinel.m_pLeft = &m_Sentinel;
		m_Sentinel.m_pRight = &m_Sentinel;
		
		m_pRoot = &m_Sentinel;
		
		m_NodeCount = 0;
	}
	
	// insert a value into the rbtree
	xtyRbTreeNode<T>* Insert(const T&);
	
	// delete a value from the rbtree
	void Delete(const T&);
	
	// normal visit, recursive
	void RecursiveVisit(xtyRbTreeNode<T>*) const;
	
	// layered visit, non-recursive
	void NonRecursiveVisit() const;
	
	// 2015-09-06 newly added
	xtyRbTreeNode<T>* Search(const T&);
	
public:
#ifdef DEBUG	
	friend class xtyRbTreeTest;
#endif

public:
	// added on 2016-01-24
	template<typename U> friend class xtyRbTreeIterator;
	
	// added on 2016-01-31
	template<typename __Key, typename __Value> friend class xtyMapIterator;

private:
	unsigned int m_NodeCount;
	xtyRbTreeNode<T> m_Sentinel;
	xtyRbTreeNode<T>* m_pRoot;
	
	// added on 2015-11-17
	xtyAlloc* m_pAlloc;
};

template<typename T>
xtyRbTreeNode<T>::xtyRbTreeNode()
: m_pParent(this), m_pLeft(this), m_pRight(this), m_Color(XTY_RBT_BLACK)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTreeNode::xtyRbTreeNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyRbTreeNode<T>::xtyRbTreeNode(const T& Data)
: m_Data(Data), m_pParent(NULL), m_pLeft(NULL), m_pRight(NULL), m_Color(XTY_RBT_RED)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTreeNode::xtyRbTreeNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyRbTreeNode<T>::~xtyRbTreeNode()
{
#ifdef DEBUG
	// std::cout << "In xtyRbTreeNode::~xtyRbTreeNode()" << std::endl;
#endif
	/* does nothing */
}

template<typename T>
xtyRbTree<T>::xtyRbTree(xtyAlloc* pAlloc)
: m_NodeCount(0), m_Sentinel(xtyRbTreeNode<T>()), m_pRoot(&m_Sentinel)
{
#ifdef DEBUG
	std::cout << "In xtyRbTree::xtyRbTree()" << std::endl;
#endif
	if (pAlloc == NULL)
		xtyError::ErrorExit("Invalid initialization: Null xtyAlloc*");
	
	m_pAlloc = pAlloc;
}

template<typename T>
xtyRbTree<T>::~xtyRbTree()
{
#ifdef DEBUG
	std::cout << "In xtyRbTree::~xtyRbTree()" << std::endl;
#endif
	// 2016-02-22, add dtor to avoid mem-leak
	__Clear_Aux(m_pRoot);
}

/******************************************************
    Key thoughts in rbtree-rotation:
    
        p                                    p
        |           left-rotate              |
        x          ------------->            y
       / \                                  / \
     x.l  y        <-------------          x  y.r
         / \        right-rotate          / \
       y.l y.r                          x.l x.r

    Note: in left-rotation, y.l will become x.r,
    and in right-rotation, x.r will become y.l.
     
    Three-steps(take left-rotate for example):

    1. Deal with parent-child pair1 (y.l && x)
    	x.r = y.l; (y.l == sentinel??) y.l.p = x;
    
    2. Deal with parent-child pair2 (x(y) && x.p)
        y.p = x.p; (x.p == sentinel??) x.p.l(r) = y;
    
    3. Deal with parent-child pair3 (x && y)
        y.l = x; x.p = y;
    
    Reference:
    	Introduction to Algorithm, 3rd Edition

******************************************************/

template<typename T>
void xtyRbTree<T>::LeftRotate(xtyRbTreeNode<T>* pNode)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::LeftRotate()" << std::endl;
#endif
	xtyRbTreeNode<T>* pTemp = pNode->m_pRight;
	
	// deal with parent-child pair1
	pNode->m_pRight = pTemp->m_pLeft;
	
	if (pTemp->m_pLeft != &m_Sentinel)
		pTemp->m_pLeft->m_pParent = pNode;
	
	// deal with parent-child pair2
	pTemp->m_pParent = pNode->m_pParent;
	
	// change root
	if (pNode->m_pParent == &m_Sentinel)
		m_pRoot = pTemp;

	else if (pNode == pNode->m_pParent->m_pLeft)
		pNode->m_pParent->m_pLeft = pTemp;

	else
		pNode->m_pParent->m_pRight = pTemp;
	
	// deal with parent-child pair3
	pTemp->m_pLeft = pNode;
	pNode->m_pParent = pTemp;
	
}

template<typename T>
void xtyRbTree<T>::RightRotate(xtyRbTreeNode<T>* pNode)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::RightRotate()" << std::endl;
#endif
	xtyRbTreeNode<T>* pTemp = pNode->m_pLeft;
	
	// deal with parent-child pair1
	pNode->m_pLeft = pTemp->m_pRight;
	
	if (pTemp->m_pRight != &m_Sentinel)
		pTemp->m_pRight->m_pParent = pNode;
	
	// deal with parent-child pair2
	pTemp->m_pParent = pNode->m_pParent;
	
	// change root
	if (pNode->m_pParent == &m_Sentinel)
		m_pRoot = pTemp;
	
	else if (pNode == pNode->m_pParent->m_pRight)
		pNode->m_pParent->m_pRight = pTemp;
	
	else
		pNode->m_pParent->m_pLeft = pTemp;
	
	// deal with parent-child pair3
	pTemp->m_pRight = pNode;
	pNode->m_pParent = pTemp;	
}

template<typename T>
xtyRbTreeNode<T>* xtyRbTree<T>::Insert(const T& InsertValue)
{
#ifdef DEBUG
	std::cout << "In xtyRbTree::Insert()" << std::endl;
#endif
	xtyRbTreeNode<T>* pNode = Search(InsertValue);
	
	if (pNode != NULL)
	{
#ifdef DEBUG
		std::cout << "The given data is already in the rbtree" << std::endl;
#endif
		return pNode;
	}
	
	pNode = (xtyRbTreeNode<T>*) m_pAlloc->Allocate(sizeof(xtyRbTreeNode<T>));
	pNode = new (pNode) xtyRbTreeNode<T>(InsertValue);
	
	xtyRbTreeNode<T>* p = m_pRoot;
	xtyRbTreeNode<T>* pp = &m_Sentinel;
	
	// find the insert-position
	while (p != &m_Sentinel)
	{
		pp = p;

		if (p->m_Data > pNode->m_Data)
			p = p->m_pLeft;
		
		else
			p = p->m_pRight;
	}
	
	// rbtree is empty
	if (pp == &m_Sentinel)
		m_pRoot = pNode;
	
	// rbtree is not empty
	else if (pNode->m_Data < pp->m_Data)
		pp->m_pLeft = pNode;
	
	else
		pp->m_pRight = pNode;
	
	pNode->m_pParent = pp;
	pNode->m_pLeft = &m_Sentinel;
	pNode->m_pRight = &m_Sentinel;
	
	// insert node must be red for fix-up process
	pNode->m_Color = XTY_RBT_RED;
	
	InsertFixUp(pNode);
	
	m_NodeCount++;
	
	// 2015-09-15 changed from 'void' to 'node*'
	return pNode;
}

/**************************************************************
    Key thoughts in rbtree-insert-fixup:
    (we assume p && pp are both red during the process)
    
    Always keep the number of 'black' nodes in each path
    unchanged in the rbtree (by spliting black color from 
    parent to its 2 children and rotations), run a bottom-up
    loop from leaf to root(while p && pp are both red), and
    finally dye the root node color to be black.
    
    The number of black nodes in each path will stay unchanged
    until the root node becomes black, which means the number
    of black nodes is added by 1 for all branches at the same
    time.
    
    Key Node: p's uncle
       
    Three-cases in rbtree-insert-fixup:
    
    1. p's uncle is red
    	(p's grandparent split black color to its 2 children)
    	
    	         b                          r <--- p*
    	        / \         split          / \
    	       r   r       ------->       b   b
    	      /                          /
      p ---> r                          r
    
    2. p's uncle is black and p is a right child
	(rotate p's father to get into case 3)
	
                 b                          b
                / \         rotate         / \
               r*  b       ------->       r   b  
                \                        /
          p ---> r                      r* <--- p*
    
    3. p's uncle is black and p is a left child
	(rotate p's grandparent and change color to move
	 a red node from insert-branch to sibling-branch)
                 
                 b*                         b*(r*)
                / \         rotate         / \
               r*  b       ------->       r   r*(b*)
              /                                \
      p ---> r                                  b
          
    (2 red nodes in a row)       (1 red node for each branch)
    
    Reference:
    	Introduction to Algorithm, 3rd Edition
    	         
**************************************************************/

template<typename T>
void xtyRbTree<T>::InsertFixUp(xtyRbTreeNode<T>* pNode)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::InsertFixUp()" << std::endl;
#endif
	xtyRbTreeNode<T>* pUncle;
	
	// if p's parent is red, then p definitely has grandparent
	while (pNode->m_pParent->m_Color == XTY_RBT_RED)
	{
		if (pNode->m_pParent == pNode->m_pParent->m_pParent->m_pLeft)
		{
			// p's uncle definitely exists, because of &m_Sentinel 
			pUncle = pNode->m_pParent->m_pParent->m_pRight;
			
			// case 1: p's uncle is red
			if (pUncle->m_Color == XTY_RBT_RED)
			{
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pUncle->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_pParent->m_Color = XTY_RBT_RED;
				
				pNode = pNode->m_pParent->m_pParent;
			}
			
			else
			{
				// case 2: p's uncle is black and p is a right child
				if (pNode == pNode->m_pParent->m_pRight) 
				{
					pNode = pNode->m_pParent;
					LeftRotate(pNode);
				}
				
				// case 3: p's uncle is black and p is a left child
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_pParent->m_Color = XTY_RBT_RED;
				RightRotate(pNode->m_pParent->m_pParent);
			}
		}
		
		// symmetric situation
		else
		{
			pUncle = pNode->m_pParent->m_pParent->m_pLeft;
			
			// case 1: p's uncle is red
			if (pUncle->m_Color == XTY_RBT_RED)
			{
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pUncle->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_pParent->m_Color = XTY_RBT_RED;
				
				pNode = pNode->m_pParent->m_pParent;
			}
			
			else
			{
				// case 2: p's uncle is black and p is a left child
				if (pNode == pNode->m_pParent->m_pLeft)
				{
					pNode = pNode->m_pParent;
					RightRotate(pNode);
				}
				
				// case 3: p's uncle is black and p is a right child
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_pParent->m_Color = XTY_RBT_RED;
				LeftRotate(pNode->m_pParent->m_pParent);
			}
		}
	}
	
	// fix root's color to be black
	m_pRoot->m_Color = XTY_RBT_BLACK;
}

/*********************************************************************
    Key thougnts in rbtree-delete:
    	       
    Although pNode is the node which is going to be deleted,
    its replace node's color is which really matters.    
    
    Since replace node will take delete node's old position
    after deletion, the fix position pointer is needed to mark
    replace node's old position in the rbtree for delete-fixup.
    
    In some cases, fix-pos might be sentinel node, therefore,
    fix-pos.parent is used to locate the delete position, which
    means fix-pos.parent needs to be non-null during the process.
    
    Key node: p's replace node
    
    Three-cases in rbtree-delete:
    
    1. p does not have left child
    
    	transplant p with p.right
    	(replace node is p itself, fix position is replace.right)
    	
    	       p                     p.r <--- fix-pos
    	      / \       ----->      /   \
    	     ^   p.r             p.r.l  p.r.r
    
    2. p does not have right child
    
    	tranplant p with p.left
    	(replace node is p itself, fix position is replace.left)
    	
    	       p                     p.l <--- fix-pos
    	      / \       ----->      /   \
    	   p.l   ^               p.l.l  p.l.r
    
    3. p has both left and right child
    
	first, find p's successor in p.right as replace node
	
	3.1 p's successor is directly connect to p
	    
	    tranplant p with p.successor, 
	    and connect p.left to p.successor
	    (replace node is s, fix position is s.right)
	     
	            p                         p.r(s) <--- rep-node
	           / \          ----->       /   \
	        p.l   p.r(s)              p.l     p.r.r <--- fix-pos
	             /   \     
	            ^    p.r.r
    	
    	3.2 p's successor is not directly connect to p
    	
    	    transplant p.successor with its right child,
    	    and connect p.right to p.successor,
    	    (and then do stuffs in case 3.1)
    	    transplant p with p.successor,
    	    and connect p.left to p.successor
    	    (replace node is s, fix position is s.right)
    	    
    	           p                           s <--- rep-node
    	          / \                         / \
    	       p.l   p.r                   p.l   p.r
    	            / \                         / \
    	           *   p.r.r    ----->         *   p.r.r
    	          /                           /
    	         s                           s.r <--- fix-pos
    	        / \
    	       ^   s.r
    
    Reference:
    	Introduction to Algorithm, 3rd Edition 
    
*********************************************************************/

template<typename T>
void xtyRbTree<T>::Delete(const T& DeleteValue)
{
#ifdef DEBUG
	std::cout << "In xtyRbTree::Delete()" << std::endl;
#endif
	xtyRbTreeNode<T>* pNode = Search(DeleteValue);
	
	if (pNode == NULL)
	{
#ifdef DEBUG
		std::cout << "The given data is not in the rbtree" << std::endl;
#endif
		return;
	}
	
	unsigned int color_save;
	xtyRbTreeNode<T>* pReplaceNode;
	xtyRbTreeNode<T>* pFixPosition;
	
	// case 1: pNode does not have left child
	if (pNode->m_pLeft == &m_Sentinel)
	{
		pReplaceNode = pNode;
		color_save = pReplaceNode->m_Color;
		pFixPosition = pNode->m_pRight;
		
		// Transplant operation guarantees fixposition.parent is not NULL
		TransPlant(pNode, pNode->m_pRight);
	}
	
	// case 2: pNode does not have right child
	else if (pNode->m_pRight == &m_Sentinel)
	{
		pReplaceNode = pNode;
		color_save = pReplaceNode->m_Color;
		pFixPosition = pNode->m_pLeft;
		
		// Transplant operation guarantees fixposition.parent is not NULL
		TransPlant(pNode, pNode->m_pLeft);
	}
	
	// case 3: pNode has 2 children
	else
	{
		pReplaceNode = LeftMost(pNode->m_pRight);
		color_save = pReplaceNode->m_Color;
		
		// ?? why right, why not left ??
		// pReplaceNode definitely does not have left child (leftmost node)
		pFixPosition = pReplaceNode->m_pRight;
		
		// case 3.1: pReplaceNode is directly connect to pNode
		// need to transplant pNode with pReplaceNode and connect pNode's left
		if (pReplaceNode->m_pParent == pNode)
		
			// ?? what if pFixPosition is sentinel node ??
			// make sure fixposition.parent is not NULL, especially for sentinel node
			pFixPosition->m_pParent = pReplaceNode;
		
		else
		{
			// case 3.2: pReplaceNode is indirectly connect to pNode
			// need to transplant pReplaceNode with its right child (leftmost node),
			// connect pNode's right, and also deal with the stuffs in case 3.1 
			
			// Transplant operation guarantees x.parent is not NULL
			TransPlant(pReplaceNode, pReplaceNode->m_pRight);
			
			pReplaceNode->m_pRight = pNode->m_pRight;
			pReplaceNode->m_pRight->m_pParent = pReplaceNode;
		}
		
		// replace pNode with pReplaceNode
		TransPlant(pNode, pReplaceNode);
		
		pReplaceNode->m_pLeft = pNode->m_pLeft;
		pReplaceNode->m_pLeft->m_pParent = pReplaceNode;
		
		// dye pReplaceNode's color to be pNode's color
		pReplaceNode->m_Color = pNode->m_Color;
	}
	
	// reset pNode's pointers to be null
	pNode->m_pLeft = NULL;
	pNode->m_pRight = NULL;
	pNode->m_pParent = NULL;
	
	if (color_save == XTY_RBT_BLACK)
		DeleteFixUp(pFixPosition);

	// 2016-02-22, add dtor to avoid mem-leak
	pNode->~xtyRbTreeNode();
	
	m_NodeCount--;

	// 2015-09-15 changed from 'void' to 'node*'
	// return pNode;

	// 2016-02-22 changed from 'node*' to 'void'
	return;
}

/*************************************************************************
    Key thoughts in rbtree-delete-fixup:
    (we assume pNode is always black during the process)
    
    Since a black node has been deleted, rbtree's r-b property may be
    violated. We need to add an extra 'black' to fix-position.

    During the fix-up process, we manage to get some certain cases that
    allows us to take the extra 'black' off.
    
    By (1) dyeing a red parent to be black and a black sibling to be red or
    (2) getting one more black node from sibling branch through rotation
    we can take off the extra 'black' from p and re-establish r-b property.
    
    Key node: p's sibling
    
    Four-cases in rbtree-delete-fixup:
    
    1. p's sibling is red
    
	dye sibling node to be black, parent node to be red and perform
	left-rotate to get case 2
	
		    b*                                 b*(r*)
	           / \                                 /   \
           p ---> b   r*          ----->            r*(b*)  b
	             / \                             /  \
	            b   b                   p* ---> b    b
    
    2. p's sibling is black, and has 2 black children
    
        dye sibling node to be red, move p to p's parent and return to
        loop's condition check
        
        if p's parent is red, the loop will be terminated since the number
        of black nodes on p's branch will be added by 1
        
        	    ?                               ? <--- p*
                   / \                             / \
           p ---> b   b           ----->          b   r
                     / \                             / \
                    b   b                           b   b
                
    3. p's sibling is black, p.sib.right is black, p.sib.left is red
    
    	dye sibling node to be red, p.sib.left to be black, and perform
    	right-rotate to get case 4
    	
                    ?                                  ?
                   / \                                / \
           p ---> b   b*          ----->     p* ---> b   b*(r*)
                     / \                                 /  \
                    r*   b                              b    r*(b*)
                   / \                                       /  \
                  b   b                                     b    b 
    
    4. p's sibling is black, p.sib.right is red, p.sib.left is black
    
        dye sibling node to be parent node's color, parent node to be black
        p.sib.right to be black, and perform left-rotate
        
        the loop will be terminated since p's branch gets one more black
        node through rotation
        
                    ?*                                ?*(b*)
                   / \                               /     \
           p ---> b   b*           ----->         b*(?*)    b*(r*)  
                     / \                          /  \
                    b   r*                       b    b

    Reference:
    	Introduction to Algorithm, 3rd Edition
    	
*************************************************************************/

template<typename T>
void xtyRbTree<T>::DeleteFixUp(xtyRbTreeNode<T>* pNode)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::DeleteFixUp()" << std::endl;
#endif
	xtyRbTreeNode<T>* pSibling;
	
	while ((pNode != m_pRoot) && (pNode->m_Color == XTY_RBT_BLACK))
	{
		if (pNode == pNode->m_pParent->m_pLeft)
		{
			pSibling = pNode->m_pParent->m_pRight;
			
			// case 1: sibling node is red, perform left-rotate to get case 2
			// (after processing case 1, sibling node will definitely become black)
			if (pSibling->m_Color == XTY_RBT_RED)
			{
				pSibling->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_Color = XTY_RBT_RED;
				LeftRotate(pNode->m_pParent);
				pSibling = pNode->m_pParent->m_pRight;
			}
			
			// case 2: sibling node is black, and its 2 children are also black
			// take one black off pNode and pSibling, and move pNode to pNode.parent
			// (if pNode.parent is red, the loop will be terminated)
			if ((pSibling->m_pLeft->m_Color == XTY_RBT_BLACK) && (pSibling->m_pRight->m_Color == XTY_RBT_BLACK))
			{
				pSibling->m_Color = XTY_RBT_RED;
				pNode = pNode->m_pParent;
			}
			
			else
			{
				// case 3: sibling node is black, its left child is red, right child is black
				// dye sibling.left to be black, sibling to be red and perform right-rotate to get case 4
				if (pSibling->m_pRight->m_Color == XTY_RBT_BLACK)
				{
					pSibling->m_pLeft->m_Color = XTY_RBT_BLACK;
					pSibling->m_Color = XTY_RBT_RED;
					RightRotate(pSibling);
					pSibling = pNode->m_pParent->m_pRight;
				}
				
				// case 4: sibling node is black, its right child is red(left child's color does not matter)
				// dye sibling to be parent's color, parent to be black, and sibling's right child to be black
				// pNode's branch will get one more black node and become balanced, the loop will be terminated 
				pSibling->m_Color = pNode->m_pParent->m_Color;
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pSibling->m_pRight->m_Color = XTY_RBT_BLACK;
				LeftRotate(pNode->m_pParent);
				
				// set pNode to be root to terminate the loop
				pNode = m_pRoot;
			}
		}
		
		// symmetric situation
		else
		{
			pSibling = pNode->m_pParent->m_pLeft;
			
			// case 1: sibling node is red, perform right-rotate to get case 2
			// (after processing case 1, sibling node will definitely become black)
			if (pSibling->m_Color == XTY_RBT_RED)
			{
				pSibling->m_Color = XTY_RBT_BLACK;
				pNode->m_pParent->m_Color = XTY_RBT_RED;
				RightRotate(pNode->m_pParent);
				pSibling = pNode->m_pParent->m_pLeft;
			}
			
			// case 2: sibling node is black, and its 2 children are also black
			// take one black off pNode and pSibling, and move pNode to pNode.parent
			// (if pNode.parent is red, the loop will be terminated)
			if ((pSibling->m_pLeft->m_Color == XTY_RBT_BLACK) && (pSibling->m_pRight->m_Color == XTY_RBT_BLACK))
			{
				pSibling->m_Color = XTY_RBT_RED;
				pNode = pNode->m_pParent;
			}
			
			else
			{
				// case 3: sibling node is black, its left child is black, right child is red
				// dye sibling.right to be black, sibling to be red and perform left-rotate to get case 4
				if (pSibling->m_pLeft->m_Color == XTY_RBT_BLACK)
				{
					pSibling->m_pRight->m_Color = XTY_RBT_BLACK;
					pSibling->m_Color = XTY_RBT_RED;
					LeftRotate(pSibling);
					pSibling = pNode->m_pParent->m_pLeft;
				}
				
				// case 4: sibling node is black, its left child is red(right child's color does not matter)
				// dye sibling to be parent's color, parent to be black, and sibling's left child to be black
				// pNode's branch will get one more black node and become balanced, the loop will be terminated 
				pSibling->m_Color = pNode->m_pParent->m_Color;
				pNode->m_pParent->m_Color = XTY_RBT_BLACK;
				pSibling->m_pLeft->m_Color = XTY_RBT_BLACK;
				RightRotate(pNode->m_pParent);
				
				// set pNode to be root to terminate the loop
				pNode = m_pRoot;
			}
		}
	}
	
	// fix color balance for case 2
	pNode->m_Color = XTY_RBT_BLACK;
	
	// since 'm_Sentinel.m_pParent' may be set to denote the fix-up
	// position in 'Transplant()', it has to be reset to 'm_Sentinel'
	m_Sentinel.m_pParent = &m_Sentinel;
}

template<typename T>
void xtyRbTree<T>::RecursiveVisit(xtyRbTreeNode<T>* pNode) const
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::RecursiveVisit()" << std::endl;
#endif
	if (pNode == &m_Sentinel)
		return;

	// 2015-09-15 need to overload 'opt <<' for 'T'
	std::cout << "p.data: " << pNode->m_Data << std::endl;
	std::cout << "p.color: " << pNode->m_Color << std::endl;
	
	if (pNode->m_pLeft != &m_Sentinel)
	{
		std::cout << "turn left" << std::endl;
		RecursiveVisit(pNode->m_pLeft);
	}
	
	if (pNode->m_pRight != &m_Sentinel)
	{
		std::cout << "turn right" << std::endl;
		RecursiveVisit(pNode->m_pRight);
	}
	
	std::cout << "return" << std::endl;
}

template<typename T>
void xtyRbTree<T>::NonRecursiveVisit() const
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::NonRecursiveVisit()" << std::endl;
#endif
	if (m_NodeCount == 0)
		std::cout << "Empty rbtree, nothing to visit" << std::endl;
	
	else
	{
		std::cout << "Node count: " << m_NodeCount << std::endl;
		
		int LayerCount = 0;
		int NodeThisLayer = 0;
		int NodeNextLayer = 0;
	
		xtyRbTreeNode<T>* p = m_pRoot;
		
		// 2015-05-08 use std::queue to impl layered-visit
		std::queue<xtyRbTreeNode<T>*> q;
		
		if (p != &m_Sentinel)
		{
			q.push(p);
			NodeThisLayer++;	
		}
			
		while (!q.empty())
		{
			NodeNextLayer = 0;
			
			std::cout << "Layer: " << LayerCount++ << std::endl;
			
			int i;

			for (i = 0; i < NodeThisLayer; i++)
			{
				p = q.front();
				
				// 2015-09-15 need to overload 'opt <<' for 'T'
				std::cout << "p.data: " << p->m_Data << std::endl;
				std::cout << "p.color: " << p->m_Color << std::endl;
				
				q.pop();
			
				if (p->m_pLeft != &m_Sentinel)
				{
					q.push(p->m_pLeft);
					NodeNextLayer++;
				}
				
				if (p->m_pRight != &m_Sentinel)
				{
					q.push(p->m_pRight);
					NodeNextLayer++;
				}
			}
			
			NodeThisLayer = NodeNextLayer;
		}
	}
}

template<typename T>
xtyRbTreeNode<T>* xtyRbTree<T>::Search(const T& SearchValue)
{
#ifdef DEBUG
	// std::cout << "In xtyRbTree::Search()" << std::endl;
#endif
	xtyRbTreeNode<T>* pFound = NULL;
	xtyRbTreeNode<T>* p = m_pRoot;
	
	while (p != &m_Sentinel)
	{
		if (p->m_Data == SearchValue)
		{
			pFound = p;
			break;
		}
		
		else if (p->m_Data > SearchValue)
			p = p->m_pLeft;
		
		else
			p = p->m_pRight;
	}
	
	return pFound;
}

/*
 * 2016-01-21
 * rbtree iterator is established for rbtree traversal.
 * considering that the elements of a binary search tree are
 * arranged in certain order(smaller ones go to left subtree,
 * while larger ones go to right), we can easily impl rbtree
 * iterator's behavior(like '++' or '--') based on this.
 * Reference: The Annotated STL Sources
 */

template<typename T>
class xtyRbTreeIterator
{
private:
	// Attention: this method is only used inside of class by member 'Begin()' & 'End()'
	explicit xtyRbTreeIterator(xtyRbTreeNode<T>* const pNode, xtyRbTree<T>* const pRelatedTree)
	: m_pNode((pNode == NULL ? throw XTY_NULL_POINTER : pNode)),
	m_pRelatedTree((pRelatedTree == NULL ? throw XTY_NULL_POINTER : pRelatedTree))
	{
		// this method has to guarantee that 'pNode' is in 'pRelatedTree'
	}
	
public:
	explicit xtyRbTreeIterator(xtyRbTree<T>* const pRelatedTree)
	: m_pNode((pRelatedTree == NULL ? throw XTY_NULL_POINTER : &(pRelatedTree->m_Sentinel))),
	m_pRelatedTree((pRelatedTree == NULL ? throw XTY_NULL_POINTER : pRelatedTree))
	{

	}
	
	~xtyRbTreeIterator()
	{
	
	}

public:
	xtyRbTreeIterator(const xtyRbTreeIterator& Iter)
	: m_pNode(Iter.m_pNode), m_pRelatedTree(Iter.m_pRelatedTree)
	{
		// use init-list for copy-construction
	}
	
	xtyRbTreeIterator& operator = (const xtyRbTreeIterator& Iter)
	{
		m_pNode = Iter.m_pNode;
		m_pRelatedTree = Iter.m_pRelatedTree;
		return *this;
	}

private:
	void Increment()
	{
		// null-pointer protection
		if (!m_pNode)
			throw XTY_INVALID_ITERATOR;
		
		if (m_pNode->m_pRight != &(m_pRelatedTree->m_Sentinel))
		{
			m_pNode = m_pNode->m_pRight;
			
			// find the successor of 'm_pNode' in right sub-tree
			while (m_pNode->m_pLeft != &(m_pRelatedTree->m_Sentinel))
				m_pNode = m_pNode->m_pLeft;
		}
		
		else
		{
			while (m_pNode->m_pParent != &(m_pRelatedTree->m_Sentinel) && m_pNode == m_pNode->m_pParent->m_pRight)
				m_pNode = m_pNode->m_pParent;
			
			// case 1: loop ends when 'm_pNode->m_pParent == &(m_pRelatedTree->m_Sentinel)'
			if (m_pNode->m_pParent == &(m_pRelatedTree->m_Sentinel))
				m_pNode = &(m_pRelatedTree->m_Sentinel);
			
			// case 2: loop ends when 'm_pNode == m_pNode->m_pParent->m_pLeft'
			else if (m_pNode == m_pNode->m_pParent->m_pLeft)
				m_pNode = m_pNode->m_pParent;
		}
	}

	void Decrement()
	{
		// null-pointer protection
		if (!m_pNode)
			throw XTY_INVALID_ITERATOR;
		
		if (m_pNode->m_pLeft != &(m_pRelatedTree->m_Sentinel))
		{
			m_pNode = m_pNode->m_pLeft;
			
			// find the predecessor of 'm_pNode' in left sub-tree
			while (m_pNode->m_pRight != &(m_pRelatedTree->m_Sentinel))
				m_pNode = m_pNode->m_pRight;
		}
		
		else
		{
			while (m_pNode->m_pParent != &(m_pRelatedTree->m_Sentinel) && m_pNode == m_pNode->m_pParent->m_pLeft)
				m_pNode = m_pNode->m_pParent;
			
			// case 1: loop ends when 'm_pNode->m_pParent == &(m_pRelatedTree->m_Sentinel)'
			if (m_pNode->m_pParent == &(m_pRelatedTree->m_Sentinel))
				m_pNode = &(m_pRelatedTree->m_Sentinel);
			
			// case 2: loop ends when 'm_pNode == m_pNode->m_pParent->m_pRight'
			else if (m_pNode == m_pNode->m_pParent->m_pRight)
				m_pNode = m_pNode->m_pParent;
		}
	}

public:
	T* operator -> () { return m_pNode->GetDataPtr(); }
	T& operator * () { return *(m_pNode->GetDataPtr()); }

public:
	template<typename __Key, typename __Value> friend class xtyMapIterator;
	
public:
	// ok to return 'ref_type'
	xtyRbTreeIterator& operator ++ ()
	{
		Increment();
		return *this;
	}
	
	// cannot return 'ref_type' because 'tmp' will destruct
	xtyRbTreeIterator operator ++ (int)
	{
		xtyRbTreeIterator tmp = *this;
		Increment();
		return tmp;
	}
	
	// ok to return 'ref_type'
	xtyRbTreeIterator& operator -- ()
	{
		Decrement();
		return *this;
	}
	
	// cannot return 'ref_type' because 'tmp' will destruct
	xtyRbTreeIterator operator -- (int)
	{
		xtyRbTreeIterator tmp = *this;
		Decrement();
		return tmp;
	}
	
	bool operator == (const xtyRbTreeIterator& Iter) const
	{
		return ((m_pNode == Iter.m_pNode) && (m_pRelatedTree == Iter.m_pRelatedTree));
	}
	
	bool operator != (const xtyRbTreeIterator& Iter) const
	{
		return ((m_pNode != Iter.m_pNode) || (m_pRelatedTree != Iter.m_pRelatedTree));
	}
	
public:
	xtyRbTreeIterator Begin() const
	{
		// calling the private constructor
		return xtyRbTreeIterator(m_pRelatedTree->LeftMost(m_pRelatedTree->m_pRoot), m_pRelatedTree);
	}
	
	xtyRbTreeIterator ReverseBegin() const
	{
		// calling the private constructor
		return xtyRbTreeIterator(m_pRelatedTree->RightMost(m_pRelatedTree->m_pRoot), m_pRelatedTree);
	}
	
	xtyRbTreeIterator End() const
	{
		// calling the private constructor
		return xtyRbTreeIterator(&(m_pRelatedTree->m_Sentinel), m_pRelatedTree);
	}
	
private:
	xtyRbTreeNode<T>* m_pNode;
	xtyRbTree<T>* m_pRelatedTree;
};

#endif

