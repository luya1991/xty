
#include "xty.h"

xtySessionSendJob::xtySessionSendJob(xtySession* const pSession)
: m_pSession((pSession == NULL ? throw XTY_NULL_POINTER : pSession))
{
#ifdef DEBUG
	std::cout << "In xtySessionSendJob::xtySessionSendJob()" << std::endl;
#endif
}

xtySessionSendJob::~xtySessionSendJob()
{
#ifdef DEBUG
	std::cout << "In xtySessionSendJob::~xtySessionSendJob()" << std::endl;
#endif
}

void xtySessionSendJob::RunJobRoutine(void* pContext)
{
	while (1)
	{
		// 2015-11-04, NOTE: USE 'xtyMsgLoop' TO IMPL THIS!
		// 2015-11-05, no this is the network layer, not the application layer, so we cannot use 'xtyMsg'
		
		xtyMsg* pSendMsg = m_pSession->m_pSendMsgQueue->PopMsg();
		
		size_t SerializeDataLen = 0;
		
		const char* pSerializeData = 
		m_pSession->m_pSerializer->Serialize(pSendMsg, &SerializeDataLen);
		
		if (SerializeDataLen != pSendMsg->m_MsgLen)
		{
			xtyLog::WriteLog("In xtySessionSendJob::RunJobRoutine(): msg serialization failed");
			continue;
		}
		
		if (send(m_pSession->m_SockFd, pSerializeData, SerializeDataLen, 0) == XTY_ERROR)
			xtyLog::WriteLog("In xtySessionSendJob::RunJobRoutine(): %s", strerror(errno));
		
		// release an serialize entity for reuse
		m_pSession->m_pSerializer->ReleaseEntity(pSerializeData);

		// release send msg
		delete pSendMsg;
	}
}

