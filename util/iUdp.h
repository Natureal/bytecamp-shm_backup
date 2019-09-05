#pragma once


//TODO ~

#include <cstdint>
#include "util/iMsg.h"
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>

const int MAX_EVENT_NUMBER = 100000;

class iUdp
{
private:
    struct sockaddr_in addr; // 这个addr放的是{127.0.0.1, localPort_}
    uint64_t localPort_;
    uint64_t peerPort_;
    uint64_t sockfd_;
    int32_t epollFd_, max_event_number_;
    struct epoll_event* events_;
public:
    iUdp(uint64_t localPort, uint64_t peerPort);
    ~iUdp();

    int bind();
    void send(iMsg& pkg, uint64_t peerPort);
    int recv(iMsg& pkg);
    void setPeerPort(uint64_t peerPort);
};

iUdp::iUdp(uint64_t localPort = 0, uint64_t peerPort) : localPort_(localPort), peerPort_(peerPort)
{
    sockfd_ = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    /* error catch TODO */
    if (sockfd_ < 0) throw std::exception();
    int enable = 1;
    if(setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		throw std::exception();
    epollFd_ = ::epoll_create(1);
    if(epollfd < 0)
		throw std::exception();
    
    events_ = new struct epoll_event[MAX_EVENT_NUMBER];
    if(nullptr == events_) throw std::exception();
    addr.sin_family = AF_UNIX;
    addr.sin_port = htons(localPort_);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (::bind(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) == EOF){
        /* error catch TODO */
        std::cout << "bind error" << std::endl;
        throw std::exception();
    }

    struct epoll_event event;
    event.data.fd = sockfd_;
    event.events = EPOLLIN;
    if (EOF == ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, sockfd_, &event))
        throw std::exception();
}

iUdp::~iUdp()
{
    ::close(sockfd_);
}

void iUdp::send(iMsg& pkg, uint64_t peerPort) {
    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_UNIX;
    peerAddr.sin_port = htons(peerPort_);
    peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sendto(sockfd_, (void*)&pkg, sizeof(pkg), 0, (struct sockaddr*)&peerAddr, sizeof peerAddr);

}

// TODO FIXME
int iUdp::recv(iMsg& pkg) {
    socklen_t len = sizeof addr;
    int cnt = recvfrom(sockfd_, (void*)&pkg, sizeof(pkg), 0, (struct sockaddr*)&addr, &len);

    return cnt;
}



void iUdp::setPeerPort(uint64_t peerPort){
    peerPort_ = peerPort;
}



