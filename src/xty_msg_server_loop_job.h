
#ifndef XTY_MSG_SERVER_LOOP_JOB_H
#define XTY_MSG_SERVER_LOOP_JOB_H

class xtyMsgServerLoopJob : public xtyMsgLoop, public xtyJob
{
public:
	explicit xtyMsgServerLoopJob(xtyMsgServer* const pMsgServer);
	virtual ~xtyMsgServerLoopJob();

private:
	xtyMsgServerLoopJob(const xtyMsgServerLoopJob&);
	xtyMsgServerLoopJob& operator = (const xtyMsgServerLoopJob&);
	
public:
	virtual void RunJobRoutine(void* pContext = NULL);
	
private:
	xtyMsgServer* const m_pMsgServer;
};

#endif

