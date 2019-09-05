/*
基于UNIX Domain Socket的本机通信，使用AF_UNIX方式，数据形式为流式套接字SOCK_STREAM
客户端：创建socket—绑定文件（端口）—连接—发送/接收数据—…—关闭
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>  
#include <iostream>

class AF_UNIX_client
{
public:
    int fd;
    char *socket_path;
    struct sockaddr_in in;
public:
    AF_UNIX_client(/* args */);
    AF_UNIX_client(char *addr);
    bool client_init();
    int client_connect();
    int client_read(char *buf, int size);
    int client_write(char *buf, int size);
    //~AF_UNIX_client();
};

AF_UNIX_client::AF_UNIX_client(/* args */)
{
}

AF_UNIX_client::AF_UNIX_client(char *addr) {
    socket_path = addr;
    fd = 0;
}

bool AF_UNIX_client::client_init() {
    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("client socket failed \n");
    }

    memset(&in, 0, sizeof(in));
    // un.sun_family = AF_UNIX;
    in.sin_family = AF_INET;
    unsigned int iremote_addr;
    in.sin_addr.s_addr = htons(1244);
    // inet_pton(AF_INET, "127.0.0.1", &iremote_addr);
    // in.sin_addr.s_addr = iremote_addr;
    in.sin_port = htons(9939);
    // strncpy(un.sun_path, socket_path, sizeof(un.sun_path) - 1);

    return true;
}

int AF_UNIX_client::client_connect() {
    int n = connect(fd, (struct sockaddr *) &in, sizeof(in));
    if(n < 0) {
        printf("client connect failed \n");
        exit(1);
    }
    return n;
}

int AF_UNIX_client::client_write(char *buf, int size) {
    int n = write(fd, buf, size);

    return n;
}

int AF_UNIX_client::client_read(char *buf, int size) {
    int n = read(fd, buf, size);

    return n;
}


int main() {
    
    char *p = (char*)"socket.address";

    const int size = 4096 * 16;
    char readBuffer[size];
    char writeBuffer[size];

    AF_UNIX_client cli(p);   //传入一个char*
    cli.client_init();
    cli.client_connect();



    while(1) {
        if(cli.client_read(readBuffer, size) < 0) {
            close(cli.fd);
            return 0;
        }
        // if(cli.client_write(readBuffer, size) < 0) {
        //     close(cli.fd);
        //     return 0;
        // }
    }
}
