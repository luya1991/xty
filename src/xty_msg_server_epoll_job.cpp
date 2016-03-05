
#include "xty.h"

xtyMsgServerEpollJob::xtyMsgServerEpollJob(xtyMsgServer* const pMsgServer)
: m_pMsgServer((pMsgServer == NULL ? throw XTY_NULL_POINTER : pMsgServer))
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerEpollJob::xtyMsgServerEpollJob()" << std::endl;
#endif
}

xtyMsgServerEpollJob::~xtyMsgServerEpollJob()
{
#ifdef DEBUG
	std::cout << "In xtyMsgServerEpollJob::~xtyMsgServerEpollJob()" << std::endl;
#endif
}

void xtyMsgServerEpollJob::RunJobRoutine(void* pContext)
{
	int connfd, epollfd;
	struct epoll_event event_setting;
	struct epoll_event event_array[XTY_EPOLL_MAX_EVENTS];
	
	epollfd = epoll_create(XTY_EPOLL_MAX_EVENTS);

	memset(&event_setting, 0, sizeof(struct epoll_event));
	memset(event_array, 0, sizeof(struct epoll_event) * XTY_EPOLL_MAX_EVENTS);

	// 2016-02-03, I found that 'EPOLLET' will cause packet-loss(might because the thread
	// spends too much time on serialization and mutex stuff), so I use 'EPOLLLT' instead
	event_setting.events = EPOLLIN;
	event_setting.data.fd = m_pMsgServer->m_SockFd;
	
	xty::SetFdNonBlocking(m_pMsgServer->m_SockFd);
	
	// add msg-server listen-socket to epoll fd management
	epoll_ctl(epollfd, EPOLL_CTL_ADD, m_pMsgServer->m_SockFd, &event_setting);

	while (1)
	{
		int i, ready_events;

		ready_events = epoll_wait(epollfd, event_array, XTY_EPOLL_MAX_EVENTS, -1);

		if ((ready_events == -1) && (errno == EINTR))
			continue;

		else if (ready_events == -1)
		{
			std::cout << "epoll_wait error: ";
			std::cout << strerror(errno) << std::endl;
			exit(-1);
		}

		for (i = 0; i < ready_events; i++)
		{
			// new connection request
			if (event_array[i].data.fd == m_pMsgServer->m_SockFd && event_array[i].events & EPOLLIN)
			{
				sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);

				// need to use the original 'accept' to handle 'EWOULDBLOCK'
				if ((connfd = accept(m_pMsgServer->m_SockFd, (struct sockaddr*)&client_addr, &client_len)) < 0)
				{
					if (connfd == EWOULDBLOCK)
						continue;

					else
					{
						std::cout << "accept error: ";
						std::cout << strerror(errno) << std::endl;
						exit(-1);
					}
				}

				std::cout << "a connection has been established on fd: " << connfd << std::endl;
				
				// get current active client count
				// (no need to lock-up because current thread is the only thread
				// which could add/remove a user to/from the active client table)
				if (m_pMsgServer->m_ActiveClientTable.Size() >= XTY_EPOLL_MAX_EVENTS)
				{
					std::cout << "too many active clients, closing fd: " << connfd << std::endl;
					close(connfd);
					continue;
				}

				// allocate a new client entity for incoming data
				xtyClientEntity* pClientEntity = new xtyClientEntity(connfd);
				
				// added on 2016-01-13
				char client_ip[XTY_IP_STR_LEN] = {0};
				uint16_t client_port = 0;
				
				// get client's ip address
				if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, XTY_IP_STR_LEN) == NULL)
					xtyError::ErrorExit(errno);

				// get client's port number
				client_port = ntohs(client_addr.sin_port);
				
				// 2016-02-03 issue: valgrind pops warning info mem-leak on 'std::string::operator+'??
				// 2016-02-23 solved: because old 'xtyMap' didn't call node's dtor and clean them up!!
				std::string ClientIPPort = std::string(client_ip) + ":" + xty::Uint16ToString(client_port);
				std::cout << "client's network address info: " << ClientIPPort << std::endl;
				
				m_pMsgServer->m_ClientTableLock.Lock();
				
				// add user to active client table
				m_pMsgServer->m_ActiveClientTable[ClientIPPort] = pClientEntity;
				
				m_pMsgServer->m_ClientTableLock.UnLock();
				
				// 2016-02-03, I found that 'EPOLLET' will cause packet-loss(might because the thread
				// spends too much time on serialization and mutex stuff), so I use 'EPOLLLT' instead
				event_setting.events = EPOLLIN;
				event_setting.data.fd = connfd;
				
				xty::SetFdNonBlocking(connfd);
				
				// add client connect-socket to epoll fd management
				epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event_setting);
			}

			else
			{
				int clientfd = -1;
				char* recvbuf = NULL;
				std::string ClientIPPort;
				
				// search 'event_array[i].data.fd' in 'm_ActiveClientTable'
				xtyMapIterator<std::string, xtyClientEntity*> Iter(&m_pMsgServer->m_ActiveClientTable);
				
				m_pMsgServer->m_ClientTableLock.Lock();
				
				for (Iter = Iter.Begin(); Iter != Iter.End(); ++Iter)
				{
					if (Iter->Second && Iter->Second->m_ClientFd == event_array[i].data.fd)
					{
						clientfd = event_array[i].data.fd;
						recvbuf = Iter->Second->m_pRecvBuffer;
						ClientIPPort = Iter->First;
						break;
					}
				}
				
				m_pMsgServer->m_ClientTableLock.UnLock();
				
				// cannot find 'event_array[i].data.fd' in 'm_ActiveClientTable'
				if (clientfd == -1 || recvbuf == NULL)
				{
					std::cout << "cannot find event_array[i].data.fd: " << event_array[i].data.fd;
					std::cout << " in active client table, this fd could be invalid" << std::endl;
					continue;
				}
				
				// read event on clientfd
				if (event_array[i].events & EPOLLIN)
				{
					// init 'readbytes'
					ssize_t readbytes = 0;
					
					// clear client's recv buffer every time before 'read'
					memset(recvbuf, 0, XTY_CLIENT_BUFFER_SIZE);
					
					// need to use the original 'read' to handle 'EWOULDBLOCK'
					if ((readbytes = read(clientfd, recvbuf, XTY_CLIENT_BUFFER_SIZE)) < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							std::cout << "read socket error: ";
							std::cout << strerror(errno) << std::endl;
							exit(-1);
						}
					}

					// handle 'EOF' on clientfd
					else if (readbytes == 0)
					{
						epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, &event_setting);
						std::cout << "client has closed the connection" << std::endl;
						
						close(clientfd);
						
						m_pMsgServer->m_ClientTableLock.Lock();
						
						// release the memory of client entity
						delete m_pMsgServer->m_ActiveClientTable[ClientIPPort];
						m_pMsgServer->m_ActiveClientTable[ClientIPPort] = NULL;
						
						// remove user from active client table
						m_pMsgServer->m_ActiveClientTable.Delete(ClientIPPort);
						
						m_pMsgServer->m_ClientTableLock.UnLock();
					}

					// normal state, deserialize and push msg into recv-queue
					else
					{
						/*
						 * 2016-02-23
						 * the following stuff may cost too much time that if I
						 * set 'EPOLLET' flag to run the event-loop, it seems
						 * that some packets will be lost. may need to refer to
						 * some open-source projects to fix this problem.
						 */
						
						try
						{	
							xtyMsg* pDeSerializedMsg =
							m_pMsgServer->m_pDeSerializer->DeSerialize(recvbuf, readbytes);
							
							m_pMsgServer->m_pRecvMsgQueue->PushMsg(pDeSerializedMsg);
						}
						
						catch (xtyException& e)
						{
							e.Dump();
						}
					}
				}
			}
		}
	}

	// exiting
	close(epollfd);
}

