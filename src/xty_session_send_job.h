
#ifndef XTY_SESSION_SEND_JOB_H
#define XTY_SESSION_SEND_JOB_H

class xtySessionSendJob : public xtyJob
{
public:
	explicit xtySessionSendJob(xtySession* const pSession);
	virtual ~xtySessionSendJob();

private:
	xtySessionSendJob(const xtySessionSendJob&);
	xtySessionSendJob& operator = (const xtySessionSendJob&);

public:
	virtual void RunJobRoutine(void* pContext = NULL);

private:
	xtySession* const m_pSession;
};

#endif

