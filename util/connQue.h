#pragma once
#include "util/iAddr.h"

class connQue
{
private:
    /* data */
    iAddr iAddr;
    uint64_t shmId;
public:
    connQue(/* args */);
    ~connQue();
    uint64_t getAddr() const;
    uint64_t getShmId() const;
};

connQue::connQue(/* args */)
{
    
}

connQue::~connQue()
{
}

uint64_t connQue::getAddr() const {
    return this->iAddr.getAddr();
}

uint64_t connQue::getShmId() const {
    return this->shmId;
}
