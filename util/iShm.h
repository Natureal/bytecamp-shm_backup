#pragma once

#include <cstdint>
#include <stdio.h>
#include <string.h>
#include <iostream>

#define server_writeCursor (pBuffer)
#define server_readCursor (pBuffer+4)
#define client_writeCursor (pBuffer+8)
#define client_readCursor (pBuffer+12)
#define server_alive (pBuffer+16)
#define client_alive (pBuffer+20)

const int DEFAULT_SHM_SIZE = 4096 * 32;

class iShm
{
public:
    char* pBuffer;

    uint32_t server_lens;
    uint32_t client_lens;
    void uint2char(char* p_addr, uint32_t addr);
    uint32_t char2uint(char* p_addr);
    const uint32_t offSet = 24;

public:
    iShm();
    iShm& operator=(const iShm& tmp);
    iShm(const iShm& tmp);
    iShm(char *p, uint32_t size, uint32_t size_server = -1, uint32_t size_client = -1, bool is_server=false);
    uint32_t get_writeCursor(bool is_server); 
    uint32_t get_readCursor(bool is_server);

    bool get_server_alive();
    void set_server_alive(bool alive);
    
    bool get_client_alive();
    void set_client_alive(bool alive);
    // 优化思路1：分块读写
    // read write --> memcpy
    void shm_memcpy(char *dst, char *src, size_t size);
    int shm_read(void *paddr, uint32_t size, bool is_server);
    int shm_write(void *paddr, uint32_t size, bool is_server);
    ~iShm();
};


