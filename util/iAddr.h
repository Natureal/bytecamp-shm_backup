#ifndef IADDR_H
#define IADDR_H

#include <unistd.h>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <cstdint>

using std::string;

class iAddr {
private:
    int addr_i;
public:
    iAddr();
    iAddr(int addr_);
    iAddr(std::string addr_);
    ~iAddr();
    int getAddr() const{
        return addr_i;
    }
    int setAddr(int addr_);
    int setAddr(string addr_);
};

#endif
