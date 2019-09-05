#include "iMsg.h"

iMsg::iMsg()
{

}

iMsg::iMsg(int _port, int _backlog, mode_e _mode, iAddr _localAddr, iAddr _peerAddr, uint32_t _shmId)
    : port(_port),
    backlog(_backlog),
    mode(_mode),
    localAddr(_localAddr),
    peerAddr(_peerAddr),
    shmId(_shmId)
{
}

iMsg::~iMsg()
{
}