
#ifndef XTY_MSG_SERVER_SEND_JOB_H
#define XTY_MSG_SERVER_SEND_JOB_H

class xtyMsgServerSendJob : public xtyJob
{
public:
	explicit xtyMsgServerSendJob(xtyMsgServer* const pMsgServer);
	virtual ~xtyMsgServerSendJob();

private:
	xtyMsgServerSendJob(const xtyMsgServerSendJob&);
	xtyMsgServerSendJob& operator = (const xtyMsgServerSendJob&);

public:
	virtual void RunJobRoutine(void* pContext = NULL);
		
private:
	xtyMsgServer* const m_pMsgServer;
};

#endif

