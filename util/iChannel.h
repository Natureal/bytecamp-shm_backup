#pragma once

#include <stdint.h>
#include <list>
#include <sys/shm.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <fcntl.h>
#include "iAddr.h"
#include "aQueue.h"
#include "iShm.h"
#include "iMsg.h"

using std::to_string;
using std::string;

class iChannel
{
public:
    /* data */
    //int size; // server_lens 或者 client_lens
    iShm ptShm; // 共享内存通道
    iAddr localAddr; // 本机地址
    iAddr peerAddr; // 对端地址
    int udpFd;
    int shmId;
    bool is_server; // 是否是 listne 端
    sockaddr_in msgrAddr;
    socklen_t msgrAddr_len;

    aQueue<node> *acceptQueue;
    //iCache  cache;

public:
    // size 要 + 1 再进行字节对齐
    iChannel();
    iChannel(/* args */iAddr t_localAddr);
    ~iChannel();
    int init_udpFd();
    int listen(int t_backlog);
    int accept(iChannel &new_channel);
    int connect(iAddr serverAddr);
    int close();
    void send_udp(iMsg &msg, struct sockaddr_in &msgAddr, socklen_t &len);
    void receive_udp(iMsg &msg, struct sockaddr_in &msgAddr, socklen_t &len);
    int read(char* buf, uint32_t size, bool is_server);
    // 优化
    int write(char* buf, uint32_t size, bool is_server);
    // int setSize();
    // connQue getConnect() const;
    int setLocalAddr(iAddr lAddr_) {
        localAddr = lAddr_;
        return 0;
    }
};

