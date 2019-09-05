/*
基于UNIX Domain Socket的本机通信，使用AF_UNIX方式，数据形式为流式套接字SOCK_STREAM
服务器端：创建socket—绑定文件（端口）—监听—接受客户端连接—接收/发送数据—…—关闭
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h> 
#include <iostream> 
#include <sys/time.h>
#include <arpa/inet.h>


#define QLEN 8      //最大连接数

class AF_UNIX_server
{
    public :
        char *socket_path;
        int fd; //socket返回值
        struct sockaddr_in in;
        int clifd;  //accept返回值
    public:
        AF_UNIX_server();
        AF_UNIX_server(char *addr);
        bool server_init();
        int server_connect();
        int server_write(char *buf, int size);
        int server_read(char *buf, int size);
};
    
AF_UNIX_server::AF_UNIX_server() {}

AF_UNIX_server::AF_UNIX_server(char *addr) {
    socket_path = addr;

    fd = 0;
    clifd = 0;
    
    memset(&in, 0, sizeof(in));   //置0
    // un.sun_family = AF_UNIX;
    in.sin_family = AF_INET;
    in.sin_addr.s_addr = htonl(INADDR_ANY);
    in.sin_port = htons(9666);
    // strncpy(un.sun_path, socket_path, sizeof(un.sun_path)-1);
    unlink(socket_path);

}

//初始化
bool AF_UNIX_server::server_init() {
    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("server socket failed \n");
        exit(1);
    }

    if(bind(fd, (struct sockaddr *) &in, sizeof(in) ) < 0) {
        printf("server bind failed \n");
        exit(1);
    }

    if(listen(fd, QLEN) < 0) {
        printf("server listen failed \n");
        exit(1);
    }

    return true;
}

int AF_UNIX_server::server_connect() {
    if((clifd = accept(fd, NULL, NULL)) == -1) {
        printf("server accept wrong \n");
        exit(-1);
    }
    return clifd;
}

//服务端读取数据
int AF_UNIX_server::server_read(char *buf, int size) {
    int n;
    n = read(clifd, buf, size);
    //打印buf内容
    return n;
}

//服务端写入数据
int AF_UNIX_server::server_write(char *buf, int size) {
    int n;
    n = write(clifd, buf, size);

    //打印内容
    return n;

}


int main() {

    char *p= (char *)"socket.address";
    
    const int size = 4096 * 16;
    char readBuffer[size];
    char writeBuffer[size];

    AF_UNIX_server ser(p);

    struct timeval start;
    struct timeval end;
    long used_time = 0L;
    long total_trffic = 0L;  //总流量
    long num_traffic = 0L;   //传输次数

    ser.server_init();
    ser.server_connect();

    const int top = 1e8;

    gettimeofday(&start, NULL);
    //long stop_time = (long)30 * 1000;
    while(true) {
       
        total_trffic += ser.server_write(writeBuffer, size);
        if(total_trffic >= 1e8) break;
        //num_traffic++;
        // total_trffic += ser.server_read(readBuffer, size);       

        //gettimeofday(&end, NULL);
        //used_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;   //换成ms
    }

    gettimeofday(&end, NULL);

    used_time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;   //换成ms
    

    close(ser.fd);
    printf("每次读写长度：%d\n", size);
    printf("总时间(ms): %ld\n", (used_time));
    printf("总流量B): %ld\n", (total_trffic));
    printf("速率(MB/s): %f\n", 1000.0 * total_trffic / 1024.0 / 1024.0 / used_time);
    printf("传输次数(K)： %ld\n", num_traffic / 1000);
    return 0;
}
