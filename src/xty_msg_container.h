
#ifndef XTY_MSG_CONTAINER_H
#define XTY_MSG_CONTAINER_H

class xtyMsgContainer
{
public:
	xtyMsgContainer()
	{/* does nothing but need a definition */}

	virtual ~xtyMsgContainer()
	{/* does nothing but need a definition */}

private:
	// does not allow copying
	xtyMsgContainer(const xtyMsgContainer&);
	xtyMsgContainer& operator = (const xtyMsgContainer&);

public:
	virtual xtyMsg* GetMsg() = 0;
	virtual void PostMsg(xtyMsg*) = 0;
};

#endif

