
#include "xty.h"

xtyMsgServerSendJob::xtyMsgServerSendJob(xtyMsgServer* const pMsgServer)
: m_pMsgServer((pMsgServer == NULL ? throw XTY_NULL_POINTER : pMsgServer))
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerSendJob::xtyMsgServerSendJob()" << std::endl;
#endif
}

xtyMsgServerSendJob::~xtyMsgServerSendJob()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerSendJob::~xtyMsgServerSendJob()" << std::endl;
#endif
}

void xtyMsgServerSendJob::RunJobRoutine(void* pContext)
{
	while (1)
	{
		// 2016-02-03, have to make sure that send-queue contains 'xtyDataMsg' only
		xtyDataMsg* pSendMsg = dynamic_cast<xtyDataMsg*>(m_pMsgServer->m_pSendMsgQueue->PopMsg());
		
		std::string ToProcessIP = pSendMsg->m_ToProcessIP;
		std::string ToProcessName = pSendMsg->m_ToProcessName;
		
		xtyMapIterator<std::string, xtyClientEntity*> Iter(&m_pMsgServer->m_ActiveClientTable);
		
		m_pMsgServer->m_ClientTableLock.Lock();
		
		for (Iter = Iter.Begin(); Iter != Iter.End(); ++Iter)
		{
			if (!Iter->Second)
			{
				std::cout << "In xtyMsgServerSendJob::RunJobRoutine(): null xtyClientEntity*" << std::endl;
				continue;
			}
			
			std::string ClientIP = Iter->Second->m_ClientIP;
			std::string ClientName = Iter->Second->m_ClientName;
			
			// find the match of client-info in the map table
			if (ClientIP == ToProcessIP && ClientName == ToProcessName)
			{
				size_t SerializeDataLen = 0;

				const char* pSerializeData =
				m_pMsgServer->m_pSerializer->Serialize(pSendMsg, &SerializeDataLen);
				
				if (SerializeDataLen != pSendMsg->m_MsgLen)
				{
					xtyLog::WriteLog("In xtyMsgServerSendJob::RunJobRoutine(): msg serialization failed");
					continue;
				}
				
				int ClientFd = Iter->Second->m_ClientFd;
				
				// send data to client
				if (send(ClientFd, pSerializeData, SerializeDataLen, 0) == XTY_ERROR)
					xtyLog::WriteLog("In xtyMsgServerSendJob::RunJobRoutine(): %s", strerror(errno));
				
				// release an serialize entity for reuse
				m_pMsgServer->m_pSerializer->ReleaseEntity(pSerializeData);

				// release an deserialize entity for reuse
				m_pMsgServer->m_pDeSerializer->ReleaseEntity(pSendMsg);
			}
		}
		
		m_pMsgServer->m_ClientTableLock.UnLock();
	}
}

