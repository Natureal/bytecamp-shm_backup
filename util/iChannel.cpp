#include "iChannel.h"

const int DEFAULT_BACKLOG = 1024;
const int DEFAULT_CHANNEL_SIZE = 4096 * 32;

iChannel::iChannel(){}

iChannel::iChannel(iAddr t_localAddr)
{
	localAddr = iAddr(t_localAddr.getAddr());
	acceptQueue = nullptr;
	
	// msgrAddr initialization
	msgrAddr.sin_family = AF_INET;
	msgrAddr.sin_port = htons(4444);
	msgrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	msgrAddr_len = sizeof(msgrAddr);
}

iChannel::~iChannel()
{
	// munmap
	munmap((void*)acceptQueue, sizeof(aQueue<node>));
}

int iChannel::init_udpFd()
{
	udpFd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = 0;
	if(bind(udpFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind failed");
		return -1;
	}
	struct sockaddr_in connAddr;
	int len = sizeof(connAddr);
	getsockname(udpFd, (struct sockaddr*)&connAddr, (socklen_t*)&len);
	int port = ntohs(connAddr.sin_port);
	return port;
}

int iChannel::listen(int t_backlog)
{
	// 调用 listen 判断为 server 端
	is_server = true;
	// step1: 申请 aQueue
	int tid = syscall(SYS_gettid);
	string sname = string("server_") + to_string(tid);
	int shmFd = shm_open(sname.c_str(), O_CREAT | O_RDWR, 0666);
	if(shmFd == -1)
	{
		perror("Create shm for aQueue failed");
		return -1;
	}
	int aQueue_require = sizeof(node) * (t_backlog + 1) + 12;
	int ret = ftruncate(shmFd, aQueue_require);
	if(ret == -1)
	{
		perror("Fruncate shm failed");
		return -1;
	}
	void *shm_ptr = mmap(0, aQueue_require, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	acceptQueue = new aQueue<node>(shm_ptr, t_backlog + 1);

	// step2: bind udp socket
	int port = init_udpFd();
	//std::cout << "port: " << port << std::endl;
	// step3: send and receive udp message
	
	iMsg msg(port, t_backlog, SERVER_LISTEN_REQ, localAddr, 0, tid);
	send_udp(msg, msgrAddr, msgrAddr_len);
	

	iMsg r_msg;
	receive_udp(r_msg, msgrAddr, msgrAddr_len);

	::close(udpFd);

	if(r_msg.mode == ACK){
		return 1;
	}
	
	return -1;
	// timeout (to do)
}

int iChannel::connect(iAddr serverAddr)
{
	is_server = false;
	
	// udp socket
	int port = init_udpFd();
	
	// send udp 
	iMsg msg(port, 0, CLIENT_CONN_REQ, localAddr, serverAddr, 0);
	send_udp(msg, msgrAddr, msgrAddr_len);

	iMsg r_msg;
	receive_udp(r_msg, msgrAddr, msgrAddr_len);
	
	::close(udpFd);

	if(r_msg.mode == MNGR_CONN_RES){
		// got shm
		string prefix("c4_Manager_");
		string path = prefix + to_string(r_msg.shmId);
		shmId = r_msg.shmId;
		int shmFd = shm_open(path.c_str(), O_RDWR, 0666);
		int ret =ftruncate(shmFd, DEFAULT_CHANNEL_SIZE);
		void *shm_ptr = mmap(0, DEFAULT_CHANNEL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
		//iShm tmp((char*)shm_ptr, DEFAULT_CHANNEL_SIZE);
		ptShm = iShm((char*)shm_ptr, DEFAULT_CHANNEL_SIZE);
		//ptShm(tmp);
		peerAddr = serverAddr;
		ptShm.set_client_alive(true);
		
		return 1;
	}
	else if(r_msg.mode == SERVER_NOT_REGISTED){
		return -1;
	}
	else if(r_msg.mode == SERVER_AQ_FULL){
		return -2;
	}
	else if(CREATE_SHM_FAILED){
		return -3;
	}
	return -4;
}


int iChannel::close()
{
	if(is_server)
	{
		ptShm.set_server_alive(false);
		if(ptShm.get_client_alive())
		{
			return 0;
		}
	}
	else
	{
		ptShm.set_client_alive(false);
		if(ptShm.get_server_alive())
		{
			return 0;
		}
	}

	if(acceptQueue == nullptr)
	{
		// connect iChannel
		int port = init_udpFd();
		iMsg msg(port, 0, CONNECT_CLOSE, localAddr, shmId, 0);
		send_udp(msg, msgrAddr, msgrAddr_len);

		iMsg r_msg;
		receive_udp(r_msg, msgrAddr, msgrAddr_len);

		::close(udpFd);

		if(r_msg.mode == CONNECTION_CLOSED)
		{
			return 1;
		}
		return -1;

	}
	else
	{
		// listen iChannel
		int port = init_udpFd();
		iMsg msg(port, 0, LISTEN_CLOSE, localAddr, 0, 0);
		send_udp(msg, msgrAddr, msgrAddr_len);

		iMsg r_msg;
		receive_udp(r_msg, msgrAddr, msgrAddr_len);

		::close(udpFd);

		if(r_msg.mode == LISTEN_CLOSED)
		{
			return 1;
		}
		return -1;
	}
}


int iChannel::accept(iChannel &new_channel)
{
	string prefix("c4_Manager_");
	struct node nd;
	// nd: uint32_t shmId, uint32_t peerAddr
	if(acceptQueue->dequeue(nd)){
		string path = prefix + to_string(nd.shmId);
		int shmFd = shm_open(path.c_str(), O_RDWR, 0666);
		int ret = ftruncate(shmFd, DEFAULT_CHANNEL_SIZE);
		void *shm_ptr = mmap(0, DEFAULT_CHANNEL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
		new_channel.ptShm = iShm((char*)shm_ptr, DEFAULT_CHANNEL_SIZE);
		new_channel.shmId = nd.shmId;
		new_channel.localAddr = localAddr;
		new_channel.peerAddr = nd.peerAddr;
		new_channel.is_server = true;
		new_channel.ptShm.set_server_alive(true);
		return 1;
	}
	return -1;
}


void iChannel::send_udp(iMsg &msg, struct sockaddr_in &msgrAddr, socklen_t &len)
{
	sendto(udpFd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&msgrAddr, len);
}

void iChannel::receive_udp(iMsg &msg, struct sockaddr_in &msgrAddr, socklen_t &len)
{
	recvfrom(udpFd, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&msgrAddr, &len);
}

int iChannel::read(char *buf, uint32_t size, bool is_server)
{
	int r_count = ptShm.shm_read((void*)buf, size, is_server);
	//std::cout << "r_count: " << r_count << std::endl;
	if(r_count == 0)
	{
		int peerStatus;
		if(is_server) peerStatus = ptShm.get_client_alive();
		else peerStatus = ptShm.get_server_alive();
		if(peerStatus == false)
		{
			return -1;
		}
	}
	return r_count;
}

int iChannel::write(char *buf, uint32_t size, bool is_server)
{
	int w_count = ptShm.shm_write((void*)buf, size, is_server);
	if(w_count == 0)
	{
		int peerStatus;
		if(is_server) peerStatus = ptShm.get_client_alive();
		else peerStatus = ptShm.get_server_alive();
		if(peerStatus == false)
		{
			return -1;
		}
	}
	return w_count;
}




