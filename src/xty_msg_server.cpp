
#include "xty.h"

xtyMsgServer::xtyMsgServer(xtyAlloc* const pAlloc)
: m_ActiveClientTable(pAlloc), m_SockFd(-1), m_pAlloc(pAlloc)
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::xtyMsgServer()" << std::endl;
#endif
}

xtyMsgServer::~xtyMsgServer()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::~xtyMsgServer()" << std::endl;
#endif
}

void xtyMsgServer::InitCommunication()
{
	m_SockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_SockFd == -1)
		xtyError::ErrorExit(errno);
	
	const int on = 1;
	if (setsockopt(m_SockFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
		xtyError::ErrorExit(errno);
	
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	
	std::string server_ip;
	uint16_t server_port;
	
	xty::GetServerAddrConfig(server_ip, server_port);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	
	if (inet_pton(AF_INET, server_ip.data(), &server_addr.sin_addr) != 1)
		xtyError::ErrorExit(errno);

	if (bind(m_SockFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		xtyError::ErrorExit(errno);

	if (listen(m_SockFd, XTY_LISTEN_BACKLOG) == -1)
		xtyError::ErrorExit(errno);
}

void xtyMsgServer::InitServerFacilities()
{
	m_pSendMsgQueue = (xtyThreadMsgQueue*) (m_pAlloc->Allocate(sizeof(xtyThreadMsgQueue)));
	m_pSendMsgQueue = new (m_pSendMsgQueue) xtyThreadMsgQueue(m_pAlloc);
	
	m_pRecvMsgQueue = (xtyThreadMsgQueue*) (m_pAlloc->Allocate(sizeof(xtyThreadMsgQueue)));
	m_pRecvMsgQueue = new (m_pRecvMsgQueue) xtyThreadMsgQueue(m_pAlloc);
	
	m_pSerializer = (xtyMsgSerializer*) (m_pAlloc->Allocate(sizeof(xtyMsgSerializer)));
	m_pSerializer = new (m_pSerializer) xtyMsgSerializer(XTY_SERVER_SERIALIZE_ENTITY_NUM);
	
	m_pDeSerializer = (xtyMsgDeSerializer*) (m_pAlloc->Allocate(sizeof(xtyMsgDeSerializer)));
	m_pDeSerializer = new (m_pDeSerializer) xtyMsgDeSerializer(XTY_SERVER_DESERIALIZE_ENTITY_NUM);
	
	m_pMsgEpollThread = (xtyThread*) (m_pAlloc->Allocate(sizeof(xtyThread)));
	m_pMsgEpollThread = new (m_pMsgEpollThread) xtyThread("msg_server_msg_epoll");
	
	xtyMsgServerEpollJob* pEpollJob = (xtyMsgServerEpollJob*) (m_pAlloc->Allocate(sizeof(xtyMsgServerEpollJob)));
	pEpollJob = new (pEpollJob) xtyMsgServerEpollJob(this);
	
	m_pMsgEpollThread->LoadJob(pEpollJob);
	m_pMsgEpollThread->Run();
	
	m_pMsgLoopThread = (xtyThread*) (m_pAlloc->Allocate(sizeof(xtyThread)));
	m_pMsgLoopThread = new (m_pMsgLoopThread) xtyThread("msg_server_msg_loop");
	
	xtyMsgServerLoopJob* pLoopJob = (xtyMsgServerLoopJob*) (m_pAlloc->Allocate(sizeof(xtyMsgServerLoopJob)));
	pLoopJob = new (pLoopJob) xtyMsgServerLoopJob(this);
	
	m_pMsgLoopThread->LoadJob(pLoopJob);
	m_pMsgLoopThread->Run();
	
	m_pMsgSendThread = (xtyThread*) (m_pAlloc->Allocate(sizeof(xtyThread)));
	m_pMsgSendThread = new (m_pMsgSendThread) xtyThread("msg_server_msg_send");
	
	xtyMsgServerSendJob* pSendJob = (xtyMsgServerSendJob*) (m_pAlloc->Allocate(sizeof(xtyMsgServerSendJob)));
	pSendJob = new (pSendJob) xtyMsgServerSendJob(this);
	
	m_pMsgSendThread->LoadJob(pSendJob);
	m_pMsgSendThread->Run();
}

void xtyMsgServer::CleanUpServerFacilities()
{
	m_pMsgSendThread->QuitNoDelay();
	m_pMsgSendThread->~xtyThread();
	
	m_pMsgLoopThread->QuitNoDelay();
	m_pMsgLoopThread->~xtyThread();
	
	m_pMsgEpollThread->QuitNoDelay();
	m_pMsgEpollThread->~xtyThread();

	m_pSerializer->~xtyMsgSerializer();
	m_pDeSerializer->~xtyMsgDeSerializer();

	m_pSendMsgQueue->~xtyMsgQueue();
	m_pRecvMsgQueue->~xtyMsgQueue();
	
	close(m_SockFd);
}

void xtyMsgServer::Start()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::Start()" << std::endl;
#endif
	InitCommunication();
	
	InitServerFacilities();
}

void xtyMsgServer::Exit()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::Exit()" << std::endl;
#endif
	CleanUpServerFacilities();
}

void xtyMsgServer::HandleRegMsg(xtyMsg* pMsg)
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::HandleRegMsg()" << std::endl;
#endif
	// have to make sure that 'pMsg' is 'xtyRegMsg*'
	xtyRegMsg* pRegMsg = dynamic_cast<xtyRegMsg*>(pMsg);
	
	std::string UserIPPort;
	UserIPPort = pRegMsg->m_ProcessIP + ":" + pRegMsg->m_ProcessPort;
	
	m_ClientTableLock.Lock();
	
	// verify that 'UserIPPort' is in the map
	if (m_ActiveClientTable[UserIPPort])
	{
		// write client name, ip, port to 'm_ActiveClientTable'
		m_ActiveClientTable[UserIPPort]->m_ClientName = pRegMsg->m_ProcessName;
		m_ActiveClientTable[UserIPPort]->m_ClientIP = pRegMsg->m_ProcessIP;
		m_ActiveClientTable[UserIPPort]->m_ClientPort = pRegMsg->m_ProcessPort;
	}
	
	m_ClientTableLock.UnLock();
	
	// release the related entity of given msg
	m_pDeSerializer->ReleaseEntity(pMsg);
}

void xtyMsgServer::HandleDeRegMsg(xtyMsg* pMsg)
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::HandleDeRegMsg()" << std::endl;
#endif
	// have to make sure that 'pMsg' is 'xtyDeRegMsg*'
	xtyDeRegMsg* pDeRegMsg = dynamic_cast<xtyDeRegMsg*>(pMsg);
	
	std::string UserIPPort;
	UserIPPort = pDeRegMsg->m_ProcessIP + ":" + pDeRegMsg->m_ProcessPort;
	
	m_ClientTableLock.Lock();
	
	// verify that 'UserIPPort' is in the map
	if (m_ActiveClientTable[UserIPPort])
	{
		// write invalid client name, ip, port to 'm_ActiveClientTable'
		m_ActiveClientTable[UserIPPort]->m_ClientName = "##Invalid: client log out##";
		m_ActiveClientTable[UserIPPort]->m_ClientIP = "##Invalid: client log out##";
		m_ActiveClientTable[UserIPPort]->m_ClientPort = "##Invalid: client log out##";
	}
	
	m_ClientTableLock.UnLock();
	
	// release the related entity of given msg
	m_pDeSerializer->ReleaseEntity(pMsg);
}

void xtyMsgServer::HandleDataMsg(xtyMsg* pMsg)
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::HandleDataMsg()" << std::endl;
#endif
	// have to make sure that 'pMsg' is 'xtyDataMsg*'
	xtyDataMsg* pDataMsg = dynamic_cast<xtyDataMsg*>(pMsg);
	
	// simply push the msg into 'm_pSendMsgQueue'
	m_pSendMsgQueue->PushMsg(pDataMsg);
}

void xtyMsgServer::DumpActiveClientInfo()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServer::DumpActiveClientInfo()" << std::endl;
#endif
	xtyMapIterator<std::string, xtyClientEntity*> Iter(&m_ActiveClientTable);
	
	m_ClientTableLock.Lock();
	
	std::cout << "current active client: " << m_ActiveClientTable.Size() << std::endl;
	
	for (Iter = Iter.Begin(); Iter != Iter.End(); ++Iter)
	{
		std::cout << "network address: " << Iter->First << std::endl;
		
		if (Iter->Second)
		{
			std::cout << "fd: " << Iter->Second->m_ClientFd << ", ";
			std::cout << "process name: " << Iter->Second->m_ClientName << std::endl;
		}
	}
	
	m_ClientTableLock.UnLock();
}

