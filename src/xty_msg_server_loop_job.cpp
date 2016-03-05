
#include "xty.h"

xtyMsgServerLoopJob::xtyMsgServerLoopJob(xtyMsgServer* const pMsgServer)
: xtyMsgLoop((pMsgServer == NULL ? throw XTY_NULL_POINTER : pMsgServer->m_pRecvMsgQueue),
(pMsgServer == NULL ? throw XTY_NULL_POINTER : pMsgServer->m_pAlloc)), m_pMsgServer(pMsgServer)
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerLoopJob::xtyMsgServerLoopJob()" << std::endl;
#endif
}

xtyMsgServerLoopJob::~xtyMsgServerLoopJob()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerLoopJob::~xtyMsgServerLoopJob()" << std::endl;
#endif
}

void xtyMsgServerLoopJob::RunJobRoutine(void* pContext)
{
	// pass 'm_pMsgServer' as 'pContext' to the loop
	EnterMsgLoop(m_pMsgServer);
}

