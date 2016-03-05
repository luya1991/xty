
#ifndef XTY_MSG_SERVER_H
#define XTY_MSG_SERVER_H

#define XTY_LISTEN_BACKLOG 1024
#define XTY_EPOLL_MAX_EVENTS 1024

#define XTY_CLIENT_IPPORT_LEN 24
#define XTY_CLIENT_BUFFER_SIZE 4096

class xtyClientEntity
{
private:
	xtyClientEntity(const int ClientFd) : m_ClientFd(ClientFd)
	{
		m_pSendBuffer = new char[XTY_CLIENT_BUFFER_SIZE];
		memset(m_pSendBuffer, 0, XTY_CLIENT_BUFFER_SIZE);
		
		m_pRecvBuffer = new char[XTY_CLIENT_BUFFER_SIZE];
		memset(m_pRecvBuffer, 0, XTY_CLIENT_BUFFER_SIZE);
	}
	
	~xtyClientEntity()
	{
		delete [] m_pSendBuffer;
		m_pSendBuffer = NULL;
		
		delete [] m_pRecvBuffer;
		m_pRecvBuffer = NULL;
	}

private:
	xtyClientEntity(const xtyClientEntity&);
	xtyClientEntity& operator = (const xtyClientEntity&);

private:
	friend class xtyMsgServer;

private:
	friend class xtyMsgServerEpollJob;
	friend class xtyMsgServerLoopJob;
	friend class xtyMsgServerSendJob;
	
private:
	int m_ClientFd;
	char* m_pSendBuffer;
	char* m_pRecvBuffer;
	
	std::string m_ClientName;
	std::string m_ClientIP;
	std::string m_ClientPort;
};

class xtyMsgServer
{
public:
	xtyMsgServer(xtyAlloc* const pAlloc);
	~xtyMsgServer();

private:
	xtyMsgServer(const xtyMsgServer&);
	xtyMsgServer& operator = (const xtyMsgServer&);

public:
	void Start();
	void Exit();
	
	// added on 2016-02-05
	void DumpActiveClientInfo();
	
private:
	void InitCommunication();
	void InitServerFacilities();
	void CleanUpServerFacilities();
	
	void HandleRegMsg(xtyMsg* pMsg);
	void HandleDeRegMsg(xtyMsg* pMsg);
	void HandleDataMsg(xtyMsg* pMsg);
	
public:
	friend class xtyMsgServerEpollJob;
	friend class xtyMsgServerLoopJob;
	friend class xtyMsgServerSendJob;
	
	friend xtyMsgLoopState xtyRegMsgCallBackFun(xtyMsg* pMsg, void* pContext);
	friend xtyMsgLoopState xtyDeRegMsgCallBackFun(xtyMsg* pMsg, void* pContext);
	friend xtyMsgLoopState xtyDataMsgCallBackFun(xtyMsg* pMsg, void* pContext);

private:
	// process register mapping-table: <ip+port, xtyClientEntity*>
	xtyMap<std::string, xtyClientEntity*> m_ActiveClientTable;

	// mutex to protect mapping-table
	xtyMutex m_ClientTableLock;

	// socket
	int m_SockFd;
	
	// allocator
	xtyAlloc* m_pAlloc;
	
	// send msg queue
	xtyMsgQueue* m_pSendMsgQueue;
	
	// recv msg queue
	xtyMsgQueue* m_pRecvMsgQueue;
	
	// serializer
	xtyMsgSerializer* m_pSerializer;
	
	// deserializer
	xtyMsgDeSerializer* m_pDeSerializer;
	
	// dedicated msg-epoll thread
	xtyThread* m_pMsgEpollThread;
	
	// dedicated msg-loop thread
	xtyThread* m_pMsgLoopThread;
	
	// dedicated msg-send thread
	xtyThread* m_pMsgSendThread;
};

#endif

