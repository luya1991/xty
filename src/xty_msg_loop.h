
#ifndef XTY_MSG_LOOP_H
#define XTY_MSG_LOOP_H

class xtyMsgLoop
{
public:
	explicit xtyMsgLoop(xtyMsgContainer* pMsgContainer, xtyAlloc* pAlloc);
	virtual ~xtyMsgLoop();

private:
	xtyMsgLoop(const xtyMsgLoop&);
	xtyMsgLoop& operator = (const xtyMsgLoop&);
	
public:
	// bind msg type with msg call-back functions
	virtual void InitCallBackTable();
	
	// enter loop for msg processing
	virtual void EnterMsgLoop(void* pContext);
	
protected:
	xtyMsgContainer* m_pMsgContainer;
	xtyMap<xtyMsgType, xtyMsgCallBackFun> m_CallBackTable;
};

class xtyThreadMsgLoop : public xtyMsgLoop
{
public:
	explicit xtyThreadMsgLoop(xtyThreadMsgQueue* pThreadMsgQueue, xtyPool* pPool);	
	virtual ~xtyThreadMsgLoop();

private:
	xtyThreadMsgLoop(const xtyThreadMsgLoop&);
	xtyThreadMsgLoop& operator = (const xtyThreadMsgLoop&);

public:
#ifdef DEBUG
	friend class xtyMsgLoopTest;
#endif
};

class xtyProcessMsgLoop : public xtyMsgLoop
{
public:
	explicit xtyProcessMsgLoop(xtyProcessMsgQueue* pProcessMsgQueue, xtySharedPool* pSharedPool);
	virtual ~xtyProcessMsgLoop();

private:
	xtyProcessMsgLoop(const xtyProcessMsgLoop&);
	xtyProcessMsgLoop& operator = (const xtyProcessMsgLoop&);

public:
#ifdef DEBUG
	friend class xtyMsgLoopTest;
#endif
};

#endif

