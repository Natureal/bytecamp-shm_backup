#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <util/c4.cpp>


int main(){

#if 1
    iAddr iaddr = new iAddr("11.11.11.11");

    iChannel serv_channel = iChannel(size);
    c4_bind(serv_channel, iaddr);
    c4_listen(serv_channel, 100);
    iChannel clnt_channel = c4_accept(serv_channel);

    char str[] = "hello bytecamp";
    c4_write(clnt_channel, str, sizeof(str));
#endif
    

#if 0
    //创建套接字
    //将套接字和IP、端口绑定
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));


    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //向客户端发送数据
    char str[] = "Hello World!";
    write(clnt_sock, str, sizeof(str));
    write(clnt_sock, str, sizeof(str));
   
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
#endif

    return 0;
}







