
#include "xty.h"

xtySessionRecvJob::xtySessionRecvJob(xtySession* const pSession)
: m_pSession((pSession == NULL ? throw XTY_NULL_POINTER : pSession))
{
#ifdef DEBUG
	std::cout << "In xtySessionRecvJob::xtySessionRecvJob()" << std::endl;
#endif
}

xtySessionRecvJob::~xtySessionRecvJob()
{
#ifdef DEBUG
	std::cout << "In xtySessionRecvJob::~xtySessionRecvJob()" << std::endl;
#endif
}

void xtySessionRecvJob::RunJobRoutine(void* pContext)
{
	while (1)
	{
		// 2015-11-04, NOTE: USE 'xtyMsgLoop' TO IMPL THIS!
		// 2015-11-05, no this is the network layer, not the application layer, so we cannot use 'xtyMsg'
		
		int RecvBytes = 0;
		char pRecvBuf[XTY_SERIALIZE_BUF_LEN] = {0};
		
		if ((RecvBytes = recv(m_pSession->m_SockFd, pRecvBuf, XTY_SERIALIZE_BUF_LEN, 0)) == XTY_ERROR)
			xtyLog::WriteLog("In xtySessionRecvJob::RunJobRoutine(): %s", strerror(errno));
		
		else if (RecvBytes == 0)
			xtyLog::WriteLog("In xtySessionRecvJob::RunJobRoutine(): peer has closed the connection");
		
		else
		{
			// a valid msg head contains a 'type' field and a 'len' field
			if (RecvBytes < (int)(sizeof(xtyMsgType) + sizeof(size_t)))
			{
				xtyLog::WriteLog("In xtySessionRecvJob::RunJobRoutine(): recv incomplete msg head");
				continue;
			}
			
			char* pDeSerializeData = pRecvBuf;
			size_t DeSerializeDataLen = *(size_t*)(pRecvBuf + sizeof(xtyMsgType));
	
			if (RecvBytes != (int)DeSerializeDataLen)
			{
				xtyLog::WriteLog("In xtySessionRecvJob::RunJobRoutine(): recv msg data with incorrect length");
				continue;
			}
			
			try
			{
				xtyMsg* pRecvMsg =
				m_pSession->m_pDeSerializer->DeSerialize(pDeSerializeData, DeSerializeDataLen);

				m_pSession->m_pRecvMsgQueue->PushMsg(pRecvMsg);
			}

			catch (xtyException& e)
			{
				e.Dump();
			}
		}
	}
}

