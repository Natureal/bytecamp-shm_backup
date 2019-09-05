#include "mode.h"
#include "msgr.h"

extern const int DEFAULT_CHANNEL_SIZE;

msgr::msgr(/* args */)
	:free_list()
{
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(listenfd < 0)
		throw std::exception();

	int enable = 1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw std::exception();

	epollfd = epoll_create(EPOLL_SIZE);
	if(epollfd < 0)
		throw std::exception();

	max_event_number = MAX_EVENT_NUMBER;
	events = new struct epoll_event[max_event_number];
	if(events == nullptr)
		throw std::exception();
	
	shm_count = 1;
	stop_msgr = false;
}

msgr::~msgr()
{
	close(listenfd);
	close(epollfd);
}

inline void msgr::m_bind()
{
    struct sockaddr_in msgr_addr;
	bzero(&msgr_addr, sizeof(msgr_addr));
    msgr_addr.sin_family = AF_INET;
    msgr_addr.sin_port = htons(server_port);
    msgr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
	if(bind(listenfd, (struct sockaddr *)&msgr_addr, sizeof(msgr_addr)) < 0){
		cout << "Bind Failed" << endl;
		throw std::exception();
	}
}

inline void msgr::m_epoll_addfd(int fd, int event_code){
	struct epoll_event event;
	event.data.fd = fd;
	event.events = event_code;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		perror("add fd to epoll failed!\n");
	}
}

inline void msgr::m_epoll_removefd(int fd){
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0) == -1)
	{
		perror("remove fd from epoll failed\n");
	}
}

inline int msgr::m_create_shm(size_t size){
	int shm_id = ::shmget((key_t)shm_count, size, 0666 | IPC_CREAT);
	if(shm_id == -1){
		perror("create shm failed");
		return -1;
	}
	return shm_id;
}

void msgr::run()
{
	m_bind();
	
	m_epoll_addfd(listenfd, EPOLLIN);
	

	int timeout = -1;
	while(!stop_msgr)
	{
		int event_number = epoll_wait(epollfd, events, max_event_number, timeout);
		if(event_number < 0 && errno != EINTR)
		{
			perror("epoll wait failed");
			break;
		}
		//cout << "Event number: " << event_number << endl;
		for(int i = 0; i < event_number; ++i)
		{
			int fd = events[i].data.fd;
			if(events[i].events & EPOLLIN)
			{
				// UDP datagram received
				//cout << "Received UDP message" << endl;
				iMsg pkg;
				struct sockaddr_in client_addr;
				socklen_t client_addrlen = sizeof(client_addr);
				int recv_len = 0;
				recv_len = recvfrom(fd, (void*)&pkg, sizeof(pkg), 0, (struct sockaddr*)&client_addr, &client_addrlen);
				cout << "Received: package from " << pkg.port << endl;
				if(parsePkg(pkg) == -1) throw std::exception();
				
			}
			else if(events[i].events & (EPOLLHUP | EPOLLERR)){
				m_epoll_removefd(fd);
				close(fd);
			}
		}
	}
}

void msgr::send(iMsg& pkg, uint64_t peerPort) {
    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);
    peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sendto(listenfd, (void*)&pkg, sizeof(pkg), 0, (struct sockaddr*)&peerAddr, sizeof peerAddr);
}

int msgr::parsePkg(iMsg& pkg) {
	switch (pkg.mode)
	{
	case CLIENT_CONN_REQ:
	{
		/* code */
		if(!aqHash.count(pkg.peerAddr.getAddr())) {
			/* 服务端未注册listen 回包给client */ 
			iMsg backpack(pkg.port, pkg.backlog, SERVER_NOT_REGISTED, iAddr(0), iAddr(0), 0);
			send(backpack, pkg.port);
			return 0;
		}

		// acceptQueue 满了，不允许连接，回包通知给Client
		if(aqHash[pkg.peerAddr.getAddr()]->is_full()) {
			iMsg full(pkg.port, 0, SERVER_AQ_FULL, 0, 0, 0);
			send(full, pkg.port);
			return 0;
		}

		/* 匹配上了，创建共享内存 回包client 给到shmId */
		int32_t clientBufSize = pkg.localSize;
		size_t size = clientBufSize + serverBufSizeHash[pkg.peerAddr.getAddr()];
		// int32_t shmId = m_create_shm(size);
		int32_t shmId = free_list.getFreeFd();

		// shmget失败，回包给client
		// TODO
		if (shmId == -1){
			std::cout << "**** msgr::parsePkg() mode = CLIENT_CONN_REQ, shmId = -1";
			iMsg errpkg(pkg.port, 0, CREATE_SHM_FAILED, 0, 0, 0);
			send(errpkg, pkg.port);
			return 0;
		}


		node n(shmId, pkg.localAddr.getAddr());
		// 将client的shmId和addr插入Server的acceptQueue
		aqHash[pkg.peerAddr.getAddr()]->enqueue(n);
		// 连接成功，回包通知client
		iMsg backpack(pkg.port, pkg.backlog, MNGR_CONN_RES, 0, 0, shmId);
		send(backpack, pkg.port);
		
		break;
	}
	case SERVER_LISTEN_REQ:
	{
		// 貌似没有了？
		// 1 检查server是否注册过了，并置位bitmap 如果已经注册过了是不是有问题呢？ FIXME
		//std::cout << "HERE LISTEN1" << std::endl;
		//std::cout << "pkg local: " << pkg.localAddr.getAddr() << std::endl;
		// if(bm.test(pkg.localAddr.getAddr()) == false) bm.set(pkg.localAddr.getAddr());
		// 2 从pkg里拿到shmId，指示了acceptQueue，并将地址插到哈希表里
		std::string path = string("server_") + std::to_string(pkg.shmId);
		int fd = shm_open(path.c_str(), O_RDWR, 0666);
		int shmLen = sizeof(node) * (pkg.backlog + 1) + 12;
		ftruncate(fd, shmLen);
		void* ptShm = ::mmap(0, shmLen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptShm == nullptr) throw std::exception();
		//std::cout << "HERE LISTEN3" << std::endl;
		aQueue<node>* tmp = new aQueue<node>(ptShm, pkg.backlog + 1);
		aqHash[pkg.localAddr.getAddr()] = tmp;
		//std::cout << "Here!!" << aqHash.count(pkg.localAddr.getAddr()) << std::endl;
		///
		serverBufSizeHash[pkg.localAddr.getAddr()] = pkg.localSize;
		//std::cout << "HERE LISTEN4" << std::endl;
		// 4 回ack给server端
		iMsg ack(pkg.port, 0, ACK, 0, 0, 0);
		send(ack, pkg.port);
		break;
	}

	/// connect close 函数，只是关闭连接，也就是回收shmId
	case CONNECT_CLOSE:
	{
		/* code */
		// 回收shmId
		int shmId = pkg.shmId;
		free_list.freeFd(shmId);
		// 回包
		iMsg closePkg(pkg.port, 0, CONNECTION_CLOSED, 0, 0, 0);
		send(closePkg, pkg.port);
		
		break;
	}
	case LISTEN_CLOSE:
	{
		/* code */
		int32_t servAddr = pkg.localAddr.getAddr();
		aQueue<node>delQue = *(aqHash[servAddr]);
		aqHash.erase(servAddr);
		// 遍历 delQue
		for(int i = 0; ; i ++ ){
			node tmp;
			if(delQue.dequeue(tmp) == false) break;
			free_list.freeFd(tmp.shmId);
		}

		iMsg listenClosePkg(pkg.port, 0, LISTEN_CLOSED, 0, 0, 0);
		send(listenClosePkg, pkg.port);

		break;
	}
	default:
	{
		std::cout << "UNKNOWN MSG" << std::endl;
		break;
	}
	}
}
