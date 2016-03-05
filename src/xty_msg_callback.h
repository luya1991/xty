
#ifndef XTY_MSG_CALLBACK_H
#define XTY_MSG_CALLBACK_H

typedef enum
{
	XTY_MSG_LOOP_STATE_DULL = 0,
	XTY_MSG_LOOP_STATE_QUIT = 1,
}xtyMsgLoopState;

// 2015-09-21, msg call-back fun type define
typedef xtyMsgLoopState (*xtyMsgCallBackFun)(xtyMsg*, void*);

// basic call-back fun for testing
extern xtyMsgLoopState xtyDullMsgCallBackFun(xtyMsg* pMsg, void* pContext);

// basic call-back fun for testing
extern xtyMsgLoopState xtyQuitMsgCallBackFun(xtyMsg* pMsg, void* pContext);

// 2015-12-09 added, call-back fun for 'xtyRegMsg'
extern xtyMsgLoopState xtyRegMsgCallBackFun(xtyMsg* pMsg, void* pContext);

// 2015-12-09 added, call-back fun for 'xtyDeRegMsg'
extern xtyMsgLoopState xtyDeRegMsgCallBackFun(xtyMsg* pMsg, void* pContext);

// 2016-01-18 added, call-back fun for 'xtyDataMsg'
extern xtyMsgLoopState xtyDataMsgCallBackFun(xtyMsg* pMsg, void* pContext);

#endif

