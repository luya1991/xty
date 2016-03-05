
#include "xty.h"

xtyMsgQueue::xtyMsgQueue(xtyAlloc* pAlloc) : m_MsgQueue(pAlloc)
{
#ifdef DEBUG
	std::cout << "In xtyMsgQueue::xtyMsgQueue()" << std::endl;
#endif
}

xtyMsgQueue::~xtyMsgQueue()
{
#ifdef DEBUG
	std::cout << "In xtyMsgQueue::~xtyMsgQueue()" << std::endl;
#endif
}

xtyThreadMsgQueue::xtyThreadMsgQueue(xtyAlloc* pAlloc) : xtyMsgQueue(pAlloc)
{

}

xtyThreadMsgQueue::~xtyThreadMsgQueue()
{

}

void xtyThreadMsgQueue::PushMsg(xtyMsg* pPushMsg)
{
	m_Lock.Lock();
	
	m_MsgQueue.EnQueue(pPushMsg);
	
	m_Lock.UnLock();
	
	m_Event.Produce();
}

xtyMsg* xtyThreadMsgQueue::PopMsg()
{
	m_Event.Consume();
	
	m_Lock.Lock();
	
	xtyMsg* pPopMsg = m_MsgQueue.DeQueue();
	
	m_Lock.UnLock();
	
	return pPopMsg;
}

xtyProcessMsgQueue::xtyProcessMsgQueue(xtyAlloc* pAlloc) : xtyMsgQueue(pAlloc)
{

}

xtyProcessMsgQueue::~xtyProcessMsgQueue()
{

}

void xtyProcessMsgQueue::PushMsg(xtyMsg* pPushMsg)
{
	m_Lock.Lock();
	
	m_MsgQueue.EnQueue(pPushMsg);
	
	m_Lock.UnLock();
	
	m_Event.Produce();
}

xtyMsg* xtyProcessMsgQueue::PopMsg()
{
	m_Event.Consume();
	
	m_Lock.Lock();
	
	xtyMsg* pPopMsg = m_MsgQueue.DeQueue();
	
	m_Lock.UnLock();
	
	return pPopMsg;
}

