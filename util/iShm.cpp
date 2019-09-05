#include "iShm.h"

iShm::iShm() : pBuffer(nullptr){
	// pBuffer = nullptr;
    server_lens = client_lens = DEFAULT_SHM_SIZE;
}

iShm::iShm(char *p, uint32_t size, uint32_t size_server, uint32_t size_client, bool is_server)
    : pBuffer(p)
 {
    // 对过齐的 size
    // pBuffer = p;

    if(size_server != -1)
    {
        server_lens = size_server;
        client_lens = size_client;
    }
    else
    {
        server_lens = (size - offSet) / 2;
        client_lens = size - offSet - server_lens;
    }

    uint2char(server_writeCursor, offSet);
    uint2char(server_readCursor, server_lens + offSet);
    uint2char(client_writeCursor, server_lens + offSet);
    uint2char(client_readCursor, offSet);

    //printf("pBuffer: %u\n", pBuffer);
    //printf("server_writeCursor: %d\n", char2uint(server_writeCursor));
    //printf("server_readCursor: %d\n", char2uint(server_readCursor));
    //printf("client_writeCursor: %d\n", char2uint(client_writeCursor));
    //printf("client_readCursor: %d\n", char2uint(client_readCursor));    

}

iShm& iShm::operator=(const iShm& tmp)
{
    pBuffer = tmp.pBuffer;
    server_lens = tmp.server_lens;
    client_lens = tmp.client_lens;

}

iShm::iShm(const iShm& tmp): pBuffer(tmp.pBuffer)
{
    // pBuffer = tmp.pBuffer;
    server_lens = tmp.server_lens;
    client_lens = tmp.client_lens;

}

iShm::~iShm(){

}

bool iShm::get_server_alive()
{
    return char2uint(server_alive);
}

void iShm::set_server_alive(bool alive)
{
    *(server_alive) = (int)alive;
}
    
bool iShm::get_client_alive()
{
    return char2uint(client_alive);
}

void iShm::set_client_alive(bool alive)
{
    *(client_alive) = (int)alive;
}

void iShm::uint2char(char* p_addr, uint32_t addr) {
    *((uint32_t*)p_addr) = addr;
}

uint32_t iShm::char2uint(char* p_addr) {
    //printf("PTR: %u\n", p_addr);
    return * ((uint32_t *) p_addr);
}

/* 得到调用此函数的进程应该写的指针
flag区分server和client：0表示server,1表示client
*/
uint32_t iShm::get_writeCursor(bool is_server) {
    uint32_t writeCursor = 0;
    if(is_server){
        writeCursor = char2uint(server_writeCursor);
    }else{
        writeCursor = char2uint(client_writeCursor);
    }
    return writeCursor;
}

uint32_t iShm::get_readCursor(bool is_server) {
    uint32_t readCursor = 0;
    if(is_server) {
        //printf("pBuffer2: %u\n", pBuffer);
        readCursor = *((uint32_t*)(pBuffer + 4));
    }else {
        readCursor = char2uint(client_readCursor);
    }
    return readCursor;
}

void iShm::shm_memcpy(char *dst, char *src, size_t size){
    while(size--){
        *dst++ = *src++;
    }
}

int iShm::shm_read(void *paddr, uint32_t size, bool is_server) {

    //printf("Here1 read\n");
   // printf("pBuffer1: %u\n", pBuffer);

    char* p = (char*)paddr;
    //printf("Here2 read\n");
    //printf("pBuffer2: %u\n", pBuffer);
    uint32_t start = get_readCursor(is_server);   //读进程应该读到的位置
    //printf("Here3 read\n");
    uint32_t end = get_writeCursor(!is_server);      //写进程应该写到的位置
    //printf("Here4read\n");
    uint32_t lens = 0;     //内存区总长度
    uint32_t pstart = 0;   //通信共享内存开始位置
    uint32_t index = 0;    //表示读到的位置 通过 pstart + index 来定位

    if(is_server) {
        pstart = offSet + server_lens;
        lens = client_lens;   //服务器读的长度
    }else {
        pstart = offSet;
        lens = server_lens;   //客户端读的长度
    }

    uint32_t count = 0;   //待填入的内存指针
    index = start - pstart;

    //printf("start: %d\n", start);
    //printf("pstart: %d\n", pstart);
    //printf("index: %d\n", index);

    // memcpy
    //if(size >= 64){
        int remain = 0;
        if(pstart + index < end) remain = end - (pstart + index);
        else remain = lens - index + (end - pstart);
        count = (remain < size) ? remain : size;

        if(index + count > lens){
            int s1 = lens - index;
            memcpy(p, pBuffer + pstart + index, s1);
            memcpy(p, pBuffer + pstart, count - s1);
        }
        else{
            // good
            memcpy(p, pBuffer + pstart + index, size);
        }
    //}
    /*else{
        while(count < size && end != (pstart + index)) {
            //printf("read: %c\n", *(p + count));
            *(p + count) = *(pBuffer + (pstart+index));
            count++;
            index++;
            index = index % lens;
        }
    }*/

    if(is_server) {
        uint2char(server_readCursor, pstart+index);
    }else {
        uint2char(client_readCursor, pstart+index);
    }

    return count;
}

int iShm::shm_write(void *paddr, uint32_t size, bool is_server) {
    char *p = (char*)paddr;
    uint32_t start = get_writeCursor(is_server);  //写进程应该写到的地方
    uint32_t end = get_readCursor(!is_server);    //读进程应该读到的地方
    uint32_t lens = 0;     //操作内存的长度
    uint32_t pstart = 0;  //操作内存开始的位置
    uint32_t index = 0;   //内存读到的地方  用 pstart + index 定位
    
    if(is_server) {
        // start = server_writeCursor;
        // end = client_readCursor;
        pstart = offSet;
        lens = server_lens;
    } else {
        // start = client_writeCursor;
        // end = server_readCursor;
        pstart = server_lens + offSet;
        lens = client_lens;
    }



    uint32_t count = 0;
    index = start - pstart;

    //if(size >= 64){
        int remain = 0;
        if(pstart + index < end) remain = end - (pstart + index);
        else remain = lens - index + (end - pstart);
        //printf("remain: %d\n", remain);
        count = (remain < size) ? remain : size;
        if(index + count > lens){
            int s1 = lens - index;
            memcpy(pBuffer + pstart + index, p, s1);
            memcpy(pBuffer + pstart, p, count - s1);
        }
        else{
            memcpy(pBuffer + pstart + index, p, size);
        }
    //}
    /*else{
        while(count < size && (index + 1) % lens != end - pstart) {
            //printf("write: %c\n", *(p + count));
            //printf("pstart: %d\n", pstart);
            //printf("start: %d\n", start);
            //printf("wirte: %c\n", *(p + count));
            *(pBuffer + pstart + index) = *(p + count);
            
            count++;
            index++;
            index = index % lens;
        }
    }*/

    if(is_server) {
        uint2char(server_writeCursor, pstart+index);
    } else {
        uint2char(client_writeCursor, pstart+index);
    }
    return count;

}

