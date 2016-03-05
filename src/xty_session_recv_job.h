
#ifndef XTY_SESSION_RECV_JOB_H
#define XTY_SESSION_RECV_JOB_H

class xtySessionRecvJob : public xtyJob
{
public:
	explicit xtySessionRecvJob(xtySession* const pSession);
	virtual ~xtySessionRecvJob();

private:
	xtySessionRecvJob(const xtySessionRecvJob&);
	xtySessionRecvJob& operator = (const xtySessionRecvJob&);
	
public:
	virtual void RunJobRoutine(void* pContext = NULL);
	
private:
	xtySession* const m_pSession;
};

#endif

