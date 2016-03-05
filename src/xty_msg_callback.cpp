
#include "xty.h"

// basic call-back fun for testing
xtyMsgLoopState xtyDullMsgCallBackFun(xtyMsg* pMsg, void* pContext)
{
	return XTY_MSG_LOOP_STATE_DULL;
}

// basic call-back fun for testing
xtyMsgLoopState xtyQuitMsgCallBackFun(xtyMsg* pMsg, void* pContext)
{
	return XTY_MSG_LOOP_STATE_QUIT;
}

// 2015-12-09 added, call-back fun for 'xtyRegMsg'
xtyMsgLoopState xtyRegMsgCallBackFun(xtyMsg* pMsg, void* pContext)
{
	// have to make sure that 'pContext' is 'xtyMsgServer*'
	xtyMsgServer* pMsgServer = static_cast<xtyMsgServer*>(pContext);

	// add user to map-table
	pMsgServer->HandleRegMsg(pMsg);

	return XTY_MSG_LOOP_STATE_DULL;
}

// 2015-12-09 added, call-back fun for 'xtyDeRegMsg'
xtyMsgLoopState xtyDeRegMsgCallBackFun(xtyMsg* pMsg, void* pContext)
{
	// have to make sure that 'pContext' is 'xtyMsgServer*'
	xtyMsgServer* pMsgServer = static_cast<xtyMsgServer*>(pContext);

	// rm user from map-table
	pMsgServer->HandleDeRegMsg(pMsg);
	
	return XTY_MSG_LOOP_STATE_DULL;
}

// 2016-01-18 added, call-back fun for 'xtyDataMsg'
xtyMsgLoopState xtyDataMsgCallBackFun(xtyMsg* pMsg, void* pContext)
{
	// have to make sure that 'pContext' is 'xtyMsgServer*'
	xtyMsgServer* pMsgServer = static_cast<xtyMsgServer*>(pContext);
	
	// trans msg to dest user
	pMsgServer->HandleDataMsg(pMsg);

	return XTY_MSG_LOOP_STATE_DULL;
}

