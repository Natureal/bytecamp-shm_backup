#ifndef MSGR_H
#define MSGR_H

#include <unordered_map>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <exception>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <unordered_map>
#include "aQueue.h"
#include "bitmap.h"
#include "iMsg.h"
#include "freeList.h"
const int server_port = 4444;
const int LISTENQUEUE_LEN = 4096;
const int EPOLL_SIZE = 1;
const int MAX_EVENT_NUMBER = 100000;
const int DEFAULT_SHM_SIZE = 4096;

using std::cout;
using std::endl;


class msgr
{
private:
    /* data */
    /// [serverAddr, [clientAddr, shmId]] 是否可以和 cache类复用一下？
    int listenfd;
	int epollfd, max_event_number;
	struct epoll_event *events;
	int shm_count;
	bool stop_msgr;
	// 根据serverAddr 找到属于其的共享内存acceptQueue
	std::unordered_map<int32_t, aQueue<node>*> aqHash;
	// aQueue<node> hash[(long long)1 << 32];
	std::unordered_map<int32_t, int32_t> serverBufSizeHash;
	freeList free_list;
public:
    msgr(/* args */);
    ~msgr();
	void m_bind();
	void m_epoll_addfd(int fd, int event_code);
	void m_epoll_removefd(int fd);
	int m_create_shm(size_t size);
	void run();
	void send(iMsg& pkg, uint64_t peerPort);
	int parsePkg(iMsg& pkg);
};


#endif
