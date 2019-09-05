#ifndef IMSG_H
#define IMSG_H

#include "mode.h"
#include "iAddr.h"

class iMsg
{
public:
    /* data */
    int port;
    int backlog;
    mode_e mode;
    iAddr localAddr;
    iAddr peerAddr;
    int32_t localSize;
    uint32_t shmId;
public:
	iMsg();
    iMsg(int _port, int _backlog, mode_e _mode, iAddr _localAddr, iAddr _peerAddr, uint32_t _shmId);
    ~iMsg();
};



#endif
