#ifndef AQUEUE_H
#define AQUEUE_H

#include <cstdint>
#include <iostream>

#define IN
#define OUT
#define INOUT

struct node{
    int32_t shmId;
    int32_t peerAddr;

    node(int32_t _shmId = -1, int32_t _peerAddr = -1) : shmId(_shmId), peerAddr(_peerAddr) {}
};

template<class T>
class aQueue
{
private:
    /* data */
    /// 这些元数据由管理进程负责初始化
    void* addr_;
    int32_t* const read_;
    int32_t* const write_;
    int32_t* const size_; // 语意就是不包括头部的读写指针，能放n - 1个 T类型的对象 (因为长度为n的循环队列只能放n - 1 个值)
    const int32_t initialOffset_;
    T* payload_;
public:
    aQueue(void* addr, int32_t size);
    ~aQueue();
    bool is_full() const;
    bool is_empty() const;
    bool enqueue(T& val);
    bool dequeue(T& val);

};


template<class T> aQueue<T>::aQueue(void* addr, int32_t size)
    : addr_(addr), 
    read_((int32_t*)addr_),
    write_((int32_t*)addr_ + 1),
    size_((int32_t*)addr_ + 2),
    initialOffset_((sizeof(*read_) << 1) + sizeof(*read_))

{
    *size_ = size;
    // std::cout << sizeof read_ << std::endl;
    // *initialOffset_ = sizeof read_ << 2;
    payload_ = (T*)((char*)addr_ + initialOffset_);
    // std::cout << addr_ << std::endl;
    // std::cout << read_ << " " <<  *read_ << std::endl;
    // std::cout << write_ << " " << *write_ << std::endl;
    // std::cout << size_ << " " <<  *size_ << std::endl;
    // std::cout << initialOffset_ << std::endl;
    // std::cout << payload_ << std::endl;
}

template<class T> aQueue<T>::~aQueue()
{
}

template<class T> bool aQueue<T>::is_empty() const
{
    return *read_ == *write_;
}

template<class T> bool aQueue<T>::is_full() const
{
    return (*write_ + 1) % *size_ == *read_;
}

template<class T> bool aQueue<T>::enqueue(T& val) 
{
    // std::cout << val << std::endl;
    // std::cout << *write_ << std::endl;
    if(is_full()) return false;
    payload_[*write_ * sizeof(T)] = val;
    // std::cout << "===" << payload_[*write_] << std::endl;
    *write_ = (*write_ + 1) % *size_;
    return true;
}

template<class T> bool aQueue<T>::dequeue(T& val) 
{
    if(is_empty()) return false;
    val = payload_[*read_ * sizeof(T)];
    *read_ = (*read_ + 1) % *size_;
    return true;
}


#endif
