
#ifndef XTY_SESSION_H
#define XTY_SESSION_H

#define XTY_IP_STR_LEN 16

class xtySession
{
public:
	explicit xtySession(const std::string& ProcessName, xtyAlloc* pAlloc);
	~xtySession();

private:
	xtySession(const xtySession&);
	xtySession& operator = (const xtySession&);

private:
	// connect to msg-server before any msg transferring
	void InitConnectToServer();
	
	// initialize all the send and recv facility stuffs
	void InitSessionFacilities();
	
	// clean up all the send and recv facility stuffs
	void CleanUpSessionFacilities();

	// register on msg-server
	int Register();
	
	// deregister on msg-server
	int DeRegister();

public:
	// start session
	void Start();
	
	// exit session
	void Exit();
	
	// push a msg to send-thread
	void SendMsg(xtyDataMsg&);
	
	// pop a msg from recv-thread
	xtyDataMsg RecvMsg();

public:
	friend class xtySessionSendJob;
	friend class xtySessionRecvJob;

public:
#ifdef DEBUG
	friend class xtySessionTest;
#endif

private:
	int m_SockFd;

	std::string m_ProcessName;
	std::string m_ProcessIP;
	std::string m_ProcessPort;
	
	xtyAlloc* m_pAlloc;
	
	xtyThreadMsgQueue* m_pSendMsgQueue;
	xtyThreadMsgQueue* m_pRecvMsgQueue;
	
	xtyMsgSerializer* m_pSerializer;
	xtyMsgDeSerializer* m_pDeSerializer;
	
	xtyThread* m_pSendThread;
	xtyThread* m_pRecvThread;
};

#endif

