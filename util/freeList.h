#pragma once
#include <list>
#include <algorithm>
#include <string>

#define INITIALSIZE 1000
#define EXTENDSIZE 512

using std::list;
using std::string;



class freeList {
private:
    int count_;
    // 放 shmId
    list<int> free_list_;
    static string prefix;
public:
    freeList();
    ~freeList();
    int getFreeFd();
    void freeFd(int fd);
};

