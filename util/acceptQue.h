#pragma once
#include <cstdint>
#include <algorithm>

#define IN
#define OUT
#define INOUT

struct node{
    int32_t shmId;
    int32_t peerAddr;

    node(int32_t _shmId, int32_t _peerAddr) : shmId(_shmId), peerAddr(_peerAddr) {}
};

/*
+----------------------------------------------
 wrcs rdcs node node node .....................
  4    4    8    8    8
+----------------------------------------------

*/
class acceptQue
{
private:
    /* data */
    int32_t size_;
    char* addr_;
    int32_t nodeSize_;
    int32_t writeCursor_;
    int32_t readCursor_;
    int32_t initialOffset_;

    char* payload_;
    
public:
    acceptQue(IN int32_t size, IN void* addr);
    ~acceptQue();
    int32_t front(INOUT node& node);
    int32_t push(IN node& node_);
    int32_t getWriteCursor() const;
    int32_t getReadCursor() const;
    int32_t incrWriteCursor();
    int32_t incrReadCursor();
    bool is_full() const;
    bool is_empty() const;
};

int32_t acceptQue::getWriteCursor() const 
{
    return ((int32_t*)addr_)[0];
}

int32_t acceptQue::getReadCursor() const 
{
    return ((int32_t*)addr_)[1];
}

int32_t acceptQue::incrWriteCursor()
{
    if (size_ - writeCursor_ > 1 && )
    {
        /* code */
    }
    
    return 0;
}

int32_t acceptQue::incrReadCursor()
{
    return 0;
}


acceptQue::acceptQue(IN int32_t size, IN void* addr) 
    : size_(size + 1), addr_((char*)addr), nodeSize_(sizeof(struct node))
{
    initialOffset_ = sizeof writeCursor_ << 1;
    payload_ = (char*)addr_ + initialOffset_;
}

acceptQue::~acceptQue()
{
}

// 
int acceptQue::front(INOUT node& node) {
    if (is_empty()) return 1;
    /* TODO */
    readCursor_ = (readCursor_ + nodeSize_ - initialOffset_) % (size_ - initialOffset_) + initialOffset_;
    return 0;
}

bool acceptQue::is_full() const {
    return std::abs(readCursor_ - writeCursor_) % (size_ - initialOffset_) <= sizeof(struct node) ? true : false ;
}
bool acceptQue::is_empty() const {
    return writeCursor_ == readCursor_;
}
// only msgr can use this method
// OUT 1 表示异常，可能是队列满了 0 表示正常
// TODO
int32_t acceptQue::push(node& node_) {
    if (is_full()) return 1;
    memcpy(addr_ + initialOffset_ + writeCursor_, &node_, sizeof node_);
    writeCursor_ = (writeCursor_ + nodeSize_) % ;
    return 0;
}
