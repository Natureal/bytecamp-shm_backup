#include <sys/time.h>
#include <iostream>
#include <pthread.h>
#include "AF_UNIX_client.h"
#include "AF_UNIX_server.h"

int size = 32;     //每次读 写的大小
char *p = (char*) "socket.address";
int pt_num = 100;  //连接server的client数量

AF_UNIX_server ser(p);
bool is_end = false;  //线程是否能结束
bool time_start = false;  //开始计时

long total_traffic = 0L;  //传输的总数据大小
long count = 0L;   //读写总计次
struct timeval start;
struct timeval end;
long used_time = 0L;    //所用时间

void *new_connect(void *args) {

    printf("新线程建立\n");

    AF_UNIX_client cli(p);
    
    cli.client_init();
    cli.client_connect();
    ser.server_connect();

    printf("建立连接\n");

    char p_read[size];
    char p_write[size];
    while(!is_end) {
        ser.server_write(p_write, size);
        cli.client_read(p_read, size);

        if(time_start) {
            total_traffic += size;
            count++;
        }
        if(count >= 100*1000*1000) {
            is_end = true;
            gettimeofday(&end, NULL);
            used_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000 / 1000;
        } 
    }

    printf("读写结束\n");
}

int main() {
    ser.server_init();

    pthread_t tids[pt_num];
    for(int i=0; i < pt_num; i++) {
        int ret = pthread_create(&tids[i],NULL, new_connect, NULL);
        if(ret != 0) {
            printf("线程创建失败\n");
        }
    }
    gettimeofday(&start, NULL);
    time_start = true;

    while(used_time == 0L);
    printf("所用时间(s): %ld\n", used_time);
    printf("数据量(kB): %ld\n", total_traffic / 1024);
    printf("正常结束\n");
}

/*
int main() {


    
    char p_server_w[size];
    char p_server_r[size];
    char p_client_w[size];
    char p_client_r[size];

    long used_time = 0L;    //所用时间
    long total_traffic = 0L;  //传输的总数据大小

    struct timeval start;
    struct timeval end;

    //建立连接
    ser.server_init();
    cli.client_init();
    cli.client_connect();
    ser.server_connect();

    //开始计时
    gettimeofday(&start, NULL);

    for(int i=0; i<10000000; i++) {
        ser.server_write(p_server_w, size);
        total_traffic += cli.client_read(p_client_r, size);
        // cli.client_write(p_client_w, size);
        // total_traffic += ser.server_read(p_server_r, size);
    }

    //结束计时
    gettimeofday(&end, NULL);

    used_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - end.tv_usec) / 1000000;
    printf("每次读写长度：%d \n", size);
    printf("总用时(s)：%ld\n", used_time);
    printf("总流量(MB)：%ld\n", total_traffic / 1024 / 1024);

    printf("传输速率(MB/s)：%ld\n", total_traffic / used_time / 1024 / 1024);
}
*/
