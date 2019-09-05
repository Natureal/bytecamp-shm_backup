#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <util/aQueue.h>

#define MAXBACKLOG 1024


int c4_init(iChannel channel){
    return 1;
}

int c4_bind(iChannel channel, iAddr iaddr){
    channel.localAddr = iaddr;
    return 1;
}

int c4_listen(iChannel channel, int backlog){
    if(backlog > MAXBACKLOG){
        //用户自定义的值大于系统默认的最大值，报错
        return -1;
    }
    //获取可用端口号，用于和mngr进程使用UDP通信
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    // 2. 创建一个sockaddr，并将它的端口号设为0
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ADDR_ANY);
    addr.sin_port = 0;        // 若port指定为0,则调用bind时，系统会为其指定一个可用的端口号
    int ret = bind(sock, (SOCKADDR*)&addr, sizeof addr);
    struct sockaddr_in connAddr;
    int len = sizeof connAddr;
    ret = getsockname(sock, (SOCKADDR*)&connAddr, (socklen_t*)&len);
    if (0 != ret){
        result = false;
        goto END;
    }
    port = ntohs(connAddr.sin_port); // 获取端口号

    iUdp udp = new iUdp(port, 4444);
    iMsg imsg = new iMsg();
    imsg.port = port;//这里好像和iUdp初始化重合了
    //imsg.localAddr = iaddr;//bind阶段已经定义了
    imsg.peerAddr = NULL;
    imsg.mode = SERVER_LISTEN_REQ;
    imsg.backlog = backlog;
    udp.send(imsg);
    channel.is_server = true;
    return 1;
}


void c4_connect(iChannel channel, iAddr iaddr){
    //获取可用端口号，用于和mngr进程使用UDP通信
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    // 2. 创建一个sockaddr，并将它的端口号设为0
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ADDR_ANY);
    addr.sin_port = 0;        // 若port指定为0,则调用bind时，系统会为其指定一个可用的端口号
    int ret = bind(sock, (SOCKADDR*)&addr, sizeof addr);
    struct sockaddr_in connAddr;
    int len = sizeof connAddr;
    ret = getsockname(sock, (SOCKADDR*)&connAddr, (socklen_t*)&len);
    if (0 != ret){
        result = false;
        goto END;
    }
    port = ntohs(connAddr.sin_port); // 获取端口号

    iUdp udp = new iUdp(port, 4444);
    iMsg imsg = new iMsg();
    imsg.port = port;//这里好像和iUdp初始化重合了
    //imsg.localAddr = iaddr;//bind阶段已经定义了
    imsg.peerAddr = iaddr;
    imsg.mode = CLIENT_CONN_REQ;
    udp.send(imsg);
    /**/
    channel.is_server = false;
    return 1;
}

iChannel c4_accept(iChannel channel){
    node anode = new node();
    channel.acceptQueue.dequeue(anode);
    channel.shmId = anode->shmId;
    /* shmAt */
    channel.peerAddr = anode->iAddr;
    return channel;
}

void c4_write(iChannel channel, char* str, uint64_t size){
    iShm::shm_write(str, size, channel.is_server);
}

void c4_read(iChannel channel, char* str, uint64_t size){
    iShm::shm_read(str, size, channel.is_server);
}