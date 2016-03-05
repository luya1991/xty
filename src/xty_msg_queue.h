
#ifndef XTY_MSG_QUEUE_H
#define XTY_MSG_QUEUE_H

class xtyMsgQueue : public xtyMsgContainer
{
public:
	explicit xtyMsgQueue(xtyAlloc* pAlloc);
	virtual ~xtyMsgQueue();
	
private:
	xtyMsgQueue(const xtyMsgQueue&);
	xtyMsgQueue& operator = (const xtyMsgQueue&);

public:
	// push a msg into queue
	virtual void PushMsg(xtyMsg* pPushMsg) = 0;
	
	// pop a msg from queue
	virtual xtyMsg* PopMsg() = 0;

	// get a msg from the container, inherited from 'xtyMsgContainer'
	// virtual xtyMsg* GetMsg() = 0;
	
	// post a msg to the container, inherited from 'xtyMsgContainer'
	// virtual void PostMsg(xtyMsg* pMsg) = 0;
	
	/*
	 * 2015-09-10
	 * overload 'new', 'placement-new', 'delete', 'placement-delete' for syntac-sugar
	 * back in the old time, if I want to create a 'xtyMsgQueue', I have to write codes
	 * like this:
	 *
	 * 	xtyThreadMsgQueue* pQ = (xtyThreadMsgQueue*)
	 *		xtyPool::PoolAllocate(sizeof(xtyThreadMsgQueue));
	 *
	 * 	pQ = new (pQ) xtyThreadMsgQueue;
	 *
	 * while after the overloading, I can create a 'xtyMsgQueue' simply as following:
	 *
	 *	xtyThreadMsgQueue* pQ = new xtyThreadMsgQueue;
	 *
	 * notice: overloading 'placement-new' and 'placement-delete' is basically aimed for
	 * forward-compatibility(to make the old-style creation codes pass compiling correctly)
	 *
	 * 2015-11-11 abandoned
	 * static void* operator new (const size_t size)
	 * { return Alloc::Allocate(size); }
	 *
	 * static void* operator new (const size_t size, void* place)
	 * { return place; }
	 *
	 * static void operator delete (void* p)
	 * { return Alloc::DeAllocate(p); }
	 * 
	 * static void operator delete (void* p, void* place)
	 * { return; }
	 */

protected:
	xtyQueue<xtyMsg*> m_MsgQueue;
};

class xtyThreadMsgQueue : public xtyMsgQueue
{
public:
	explicit xtyThreadMsgQueue(xtyAlloc* pAlloc);
	virtual ~xtyThreadMsgQueue();
	
private:
	xtyThreadMsgQueue(const xtyThreadMsgQueue&);
	xtyThreadMsgQueue& operator = (const xtyThreadMsgQueue&);

public:
	// push a msg into queue, inherited from 'xtyMsgQueue'
	virtual void PushMsg(xtyMsg*);
	
	// pop a msg from queue, inherited from 'xtyMsgQueue'
	virtual xtyMsg* PopMsg();

	// get a msg from the container, inherited from 'xtyMsgContainer'
	virtual xtyMsg* GetMsg() { return PopMsg(); }
	
	// post a msg to the container, inherited from 'xtyMsgContainer'
	virtual void PostMsg(xtyMsg* pMsg) { PushMsg(pMsg); }
	
public:
#ifdef DEBUG
	friend class xtyMsgQueueTest;
#endif

private:
	xtyMutex m_Lock;
	xtyThreadEvent m_Event;
};

class xtyProcessMsgQueue : public xtyMsgQueue
{
public:
	explicit xtyProcessMsgQueue(xtyAlloc* pAlloc);
	virtual ~xtyProcessMsgQueue();
	
private:
	xtyProcessMsgQueue(const xtyProcessMsgQueue&);
	xtyProcessMsgQueue& operator = (const xtyProcessMsgQueue&);

public:
	// push a msg into queue, inherited from 'xtyMsgQueue'
	virtual void PushMsg(xtyMsg*);

	// pop a msg from queue, inherited from 'xtyMsgQueue'
	virtual xtyMsg* PopMsg();

	// get a msg from the container, inherited from 'xtyMsgContainer'
	virtual xtyMsg* GetMsg() { return PopMsg(); }
	
	// post a msg to the container, inherited from 'xtyMsgContainer'
	virtual void PostMsg(xtyMsg* pMsg) { PushMsg(pMsg); }
	
public:
#ifdef DEBUG
	friend class xtyMsgQueueTest;
#endif

private:
	xtySpinLock m_Lock;
	xtyProcessEvent m_Event;
};

#endif

