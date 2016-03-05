
#include "xty.h"

xtyMsgLoop::xtyMsgLoop(xtyMsgContainer* pMsgContainer, xtyAlloc* pAlloc) : m_CallBackTable(pAlloc)
{
	if (pMsgContainer == NULL)
		xtyError::ErrorExit("Invalid initialization: Null xtyMsgContainer*");

	m_pMsgContainer = pMsgContainer;
}

xtyMsgLoop::~xtyMsgLoop()
{

}

void xtyMsgLoop::InitCallBackTable()
{
	m_CallBackTable[XTY_DULL_MSG] = xtyDullMsgCallBackFun;
	m_CallBackTable[XTY_QUIT_MSG] = xtyQuitMsgCallBackFun;
	m_CallBackTable[XTY_REG_MSG] = xtyRegMsgCallBackFun;
	m_CallBackTable[XTY_DEREG_MSG] = xtyDeRegMsgCallBackFun;
	m_CallBackTable[XTY_DATA_MSG] = xtyDataMsgCallBackFun;
}

void xtyMsgLoop::EnterMsgLoop(void* pContext)
{	
	/*
	 * 2015-09-23 fatal error
	 * 'xtySharedMemory' class will also be copied during 'fork()',
	 * therefore, 'm_pEmptyStart' and 'm_pEmptyEnd' will be different
	 * in parent-proc and child-proc.
	 *
	 * the program has never crashed before(as in 'xty_msg_queue_test.cpp')
	 * is because there is always only one of the two-procs who does the
	 * alloc work(write the shared-pool), and the other just does the access
	 * work(read the shared-pool) without any allocations.
	 *
	 * however, 'InitCallBackTable()' will allocate 3 'xtyRbTreeNode' space
	 * in the shared-pool, which causes the difference of 'm_pEmptyStart' and
	 * 'm_pEmptyEnd' in parent-proc and child-proc.
	 *
	 * that will lead to child-proc's new allocations overwrites parent-proc's
	 * old allocations and 3 'xtyMsg*' data in the shared-pool will be broken.
	 *
	 * so, I cancel the template-para 'Alloc' for 'xtyMap', now the map has
	 * to be allocated by 'xtyPool', this will avoid the problem.
	 *
	 * 2015-11-24 error fixed
	 * now, I put 'm_pEmptyStart' and 'm_pEmptyEnd' also on shared memory,
	 * it look like the problem described above never bothers me any more.
	 * hence, it is fine to use 'xtySharedPool' for 'xtyMap' allocation works.
	 */
	
	// set up call-back fun table
	InitCallBackTable();
	
	while (1)
	{
		// interface-standardizing-programming
		xtyMsg* pMsg = m_pMsgContainer->GetMsg();

		/* notice: have to make sure that 'pMsg' is non-null */
		
		// added on 2015-09-21, find a proper call-back fun
		xtyMapNode<xtyMsgType, xtyMsgCallBackFun>* pMapNode = m_CallBackTable.Search(pMsg->m_MsgType);
		
		// default loop state
		xtyMsgLoopState LoopState = XTY_MSG_LOOP_STATE_DULL;
		
		// added on 2015-09-21, run the call-back fun
		if (pMapNode != NULL && pMapNode->Second != NULL)
			LoopState = pMapNode->Second(pMsg, pContext);

		// quit the loop
		if (LoopState == XTY_MSG_LOOP_STATE_QUIT)
			break;	
	}
}

xtyThreadMsgLoop::xtyThreadMsgLoop(xtyThreadMsgQueue* pThreadMsgQueue, xtyPool* pPool)
: xtyMsgLoop(pThreadMsgQueue, pPool)
{

}

xtyThreadMsgLoop::~xtyThreadMsgLoop()
{

}

xtyProcessMsgLoop::xtyProcessMsgLoop(xtyProcessMsgQueue* pProcessMsgQueue, xtySharedPool* pSharedPool)
: xtyMsgLoop(pProcessMsgQueue, pSharedPool)
{

}

xtyProcessMsgLoop::~xtyProcessMsgLoop()
{

}

