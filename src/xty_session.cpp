
#include "xty.h"

xtySession::xtySession(const std::string& ProcessName, xtyAlloc* pAlloc)
: m_SockFd(-1), m_ProcessName(ProcessName), m_pAlloc(NULL)
{
#ifdef DEBUG
	std::cout << "In xtySession::xtySession()" << std::endl;
#endif
	if (pAlloc == NULL)
		xtyError::ErrorExit("Invalid initialization: Null xtyAlloc*");

	m_pAlloc = pAlloc;
}

xtySession::~xtySession()
{
#ifdef DEBUG
	std::cout << "In xtySession::~xtySession()" << std::endl;
#endif
}

void xtySession::InitConnectToServer()
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
	
	if (connect(m_SockFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		xtyError::ErrorExit(errno);
	
	// added on 2016-01-11
	struct sockaddr_in local_addr;
	bzero(&local_addr, sizeof(local_addr));
	socklen_t local_addr_len = sizeof(local_addr);
	
	if (getsockname(m_SockFd, (struct sockaddr*)&local_addr, &local_addr_len) == -1)
		xtyError::ErrorExit(errno);
	
	char local_ip[XTY_IP_STR_LEN] = {0};
	uint16_t local_port = 0;
	
	if (inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, XTY_IP_STR_LEN) == NULL)
		xtyError::ErrorExit(errno);
	
	local_port = ntohs(local_addr.sin_port);
	
	m_ProcessIP = std::string(local_ip);
	m_ProcessPort = xty::Uint16ToString(local_port);

#ifdef DEBUG	
	std::cout << "m_ProcessIP: " << m_ProcessIP << std::endl;
	std::cout << "m_ProcessPort: " << m_ProcessPort << std::endl;
#endif
}

void xtySession::InitSessionFacilities()
{
	// create send msg queue
	m_pSendMsgQueue = (xtyThreadMsgQueue*)(m_pAlloc->Allocate(sizeof(xtyThreadMsgQueue)));
	m_pSendMsgQueue = new (m_pSendMsgQueue) xtyThreadMsgQueue(m_pAlloc);
	
	// create serialization stuff
	m_pSerializer = (xtyMsgSerializer*)(m_pAlloc->Allocate(sizeof(xtyMsgSerializer)));
	m_pSerializer = new (m_pSerializer) xtyMsgSerializer(XTY_SESSION_SERIALIZE_ENTITY_NUM);
	
	// create send thread
	m_pSendThread = (xtyThread*)(m_pAlloc->Allocate(sizeof(xtyThread)));
	m_pSendThread = new (m_pSendThread) xtyThread("session_sender");
	
	// create and load send job
	xtySessionSendJob* pSendJob = (xtySessionSendJob*)(m_pAlloc->Allocate(sizeof(xtySessionSendJob)));
	pSendJob = new (pSendJob) xtySessionSendJob(this);
	m_pSendThread->LoadJob(pSendJob);
	
	// create recv msg queue
	m_pRecvMsgQueue = (xtyThreadMsgQueue*)(m_pAlloc->Allocate(sizeof(xtyThreadMsgQueue)));
	m_pRecvMsgQueue = new (m_pRecvMsgQueue) xtyThreadMsgQueue(m_pAlloc);
	
	// create deserialization stuff
	m_pDeSerializer = (xtyMsgDeSerializer*)(m_pAlloc->Allocate(sizeof(xtyMsgDeSerializer)));
	m_pDeSerializer = new (m_pDeSerializer) xtyMsgDeSerializer(XTY_SESSION_DESERIALIZE_ENTITY_NUM);
	
	// create recv thread
	m_pRecvThread = (xtyThread*)(m_pAlloc->Allocate(sizeof(xtyThread)));
	m_pRecvThread = new (m_pRecvThread) xtyThread("session_receiver");
	
	// create and load recv job
	xtySessionRecvJob* pRecvJob = (xtySessionRecvJob*)(m_pAlloc->Allocate(sizeof(xtySessionRecvJob)));
	pRecvJob = new (pRecvJob) xtySessionRecvJob(this);
	m_pRecvThread->LoadJob(pRecvJob);
	
	// start threads
	m_pSendThread->Run();
	m_pRecvThread->Run();
}

int xtySession::Register()
{
#ifdef DEBUG
	std::cout << "In xtySession::Register()" << std::endl;
#endif
	// 2016-01-13, 'new' need to 'delete'
	xtyRegMsg* pRegMsg = new xtyRegMsg;
	
	pRegMsg->m_ProcessName = m_ProcessName;
	pRegMsg->m_ProcessIP = m_ProcessIP;
	pRegMsg->m_ProcessPort = m_ProcessPort;
	
	// have to guarantee that 'm_pSendThread' has been set up already
	// m_pSendMsgQueue->PushMsg(pRegMsg);

	size_t SerializeDataLen = 0;
	
	const char* pSerializeData = 
	m_pSerializer->Serialize(pRegMsg, &SerializeDataLen);
	
	if (SerializeDataLen != pRegMsg->m_MsgLen)
	{
		xtyLog::WriteLog("In xtySession::Register(): msg serialization failed");
		return XTY_ERROR;
	}
	
	if (send(m_SockFd, pSerializeData, SerializeDataLen, 0) == XTY_ERROR)
		xtyLog::WriteLog("In xtySession::Register(): %s", strerror(errno));
	
	// release an serialize entity for reuse
	m_pSerializer->ReleaseEntity(pSerializeData);

	// 2016-01-31, delete here
	delete pRegMsg;
	
	return XTY_OK;
}

int xtySession::DeRegister()
{
#ifdef DEBUG
	std::cout << "In xtySession::DeRegister()" << std::endl;
#endif
	// 2016-01-13, 'new' need to 'delete'
	xtyDeRegMsg* pDeRegMsg = new xtyDeRegMsg;
	
	pDeRegMsg->m_ProcessName = m_ProcessName;
	pDeRegMsg->m_ProcessIP = m_ProcessIP;
	pDeRegMsg->m_ProcessPort = m_ProcessPort;
	
	// have to guarantee that 'm_pSendThread' has been set up already
	// m_pSendMsgQueue->PushMsg(pDeRegMsg);
	
	size_t SerializeDataLen = 0;
	
	const char* pSerializeData = 
	m_pSerializer->Serialize(pDeRegMsg, &SerializeDataLen);
	
	if (SerializeDataLen != pDeRegMsg->m_MsgLen)
	{
		xtyLog::WriteLog("In xtySession::DeRegister(): msg serialization failed");
		return XTY_ERROR;
	}
	
	if (send(m_SockFd, pSerializeData, SerializeDataLen, 0) == XTY_ERROR)
		xtyLog::WriteLog("In xtySession::DeRegister(): %s", strerror(errno));
	
	// release an serialize entity for reuse
	m_pSerializer->ReleaseEntity(pSerializeData);
	
	// 2016-01-31, delete here
	delete pDeRegMsg;
	
	return XTY_OK;
}

void xtySession::CleanUpSessionFacilities()
{
#ifdef DEBUG
	std::cout << "In xtySession::CleanUpSessionFacilities()" << std::endl;
#endif
	// send signal to quit thread
	m_pSendThread->QuitNoDelay();
	m_pRecvThread->QuitNoDelay();
	
	// deconstructor for placement-new
	m_pSendThread->~xtyThread();
	m_pRecvThread->~xtyThread();
	
	m_pSerializer->~xtyMsgSerializer();
	m_pDeSerializer->~xtyMsgDeSerializer();
	
	m_pSendMsgQueue->~xtyMsgQueue();
	m_pRecvMsgQueue->~xtyMsgQueue();
	
	// close socket
	close(m_SockFd);
}

void xtySession::Start()
{
#ifdef DEBUG
	std::cout << "In xtySession::Start()" << std::endl;
#endif
	InitConnectToServer();
	
	InitSessionFacilities();
	
	Register();
}

void xtySession::Exit()
{
#ifdef DEBUG
	std::cout << "In xtySession::Exit()" << std::endl;
#endif
	DeRegister();
	
	CleanUpSessionFacilities();
}

void xtySession::SendMsg(xtyDataMsg &rSendMsg)
{
	// 'pSendMsg' will be deleted in send thread
	xtyDataMsg* pSendMsg = new xtyDataMsg(rSendMsg);

	// push send msg into send queue
	m_pSendMsgQueue->PushMsg(pSendMsg);
}

xtyDataMsg xtySession::RecvMsg()
{
	// pop recv msg from recv queue
	xtyDataMsg* pRecvMsg = dynamic_cast<xtyDataMsg*>(m_pRecvMsgQueue->PopMsg());

	// construct a temp msg
	xtyDataMsg TempMsg(*pRecvMsg);

	// release an entity for reuse
	m_pDeSerializer->ReleaseEntity(pRecvMsg);

	return TempMsg;
}

