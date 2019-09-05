#include "freeList.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


static const int DEFAULT_CHANNEL_SIZE = 4096 * 32;

using std::string;
using std::cout;
using std::endl;

string freeList::prefix = "c4_Manager_";

freeList::freeList() {
    for(count_ = 0; count_ < INITIALSIZE; count_ ++ ){
        string path = prefix + std::to_string(count_);
        int shmId = shm_open(path.c_str(), O_CREAT | O_RDWR, 0666);
        //cout << "==" << path << " -> " << shmId << " " << count_ << endl;
        ftruncate(shmId, DEFAULT_CHANNEL_SIZE);
        free_list_.push_back(shmId);
    }
}

int freeList::getFreeFd() {
    if(free_list_.empty()) {
        //cout << "=-=-=-=-=- 申请新 shmId 了～";
        for(int i = 1; i <= EXTENDSIZE; i ++ ){
            count_ += i;
            string path = prefix + std::to_string(count_);
            int shmId = shm_open(path.c_str(), O_CREAT | O_RDWR, 0666);
            //cout << "==" << path << " -> " << shmId << " " << count_ << endl;
            ftruncate(shmId, DEFAULT_CHANNEL_SIZE);
            free_list_.push_back(shmId);
        }
    }
    int ret = free_list_.front();
    free_list_.pop_front();

    return ret;
}

void freeList::freeFd(int fd){
    //cout << "--------- free: " << fd << endl;
    free_list_.push_back(fd);
}

freeList::~freeList() {
    for(auto x : free_list_){
        string path = prefix + std::to_string(x);
        shm_unlink(path.c_str());
    }
}