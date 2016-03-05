
#ifndef XTY_MSG_SERVER_EPOLL_JOB_H
#define XTY_MSG_SERVER_EPOLL_JOB_H

class xtyMsgServerEpollJob : public xtyJob
{
public:
	explicit xtyMsgServerEpollJob(xtyMsgServer* const pMsgServer);
	virtual ~xtyMsgServerEpollJob();

private:
	xtyMsgServerEpollJob(const xtyMsgServerEpollJob&);
	xtyMsgServerEpollJob& operator = (const xtyMsgServerEpollJob&);

public:
	virtual void RunJobRoutine(void* pContext = NULL);
		
private:
	xtyMsgServer* const m_pMsgServer;
};

#endif

