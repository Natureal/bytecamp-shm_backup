#include "iAddr.h"

iAddr::iAddr(){}

iAddr::~iAddr() {}

iAddr::iAddr(int addr_) : addr_i(addr_) {}
iAddr::iAddr(std::string addr_) {
    /* TODO */
    //iAddr::addr_s = addr_;
    int x = 0;
    std::string temp = "";
    for(int i=0; i < addr_.size(); i++) {
        if(addr_[i] == '.' ) {
            x = x << 8;
            x += std::stoi(temp);
            temp = "";
        }else {
            temp += addr_[i];
        }
    }
    x = x << 8;
    x += std::stoi(temp);
    addr_i = x;
}

int iAddr::setAddr(int addr_){
    addr_i = addr_;
    return addr_; 
}

int iAddr::setAddr(std::string addr_) {
    //addr_s = addr_;
    
    int x = 0;
    std::string temp = "";
    for(int i=0; i < addr_.size(); i++) {
        if(addr_[i] == '.' || i==addr_.size()-1 ) {
            x = x << 8;
            x += std::stoi(temp);
            temp = "";
        }else {
            temp += addr_[i];
        }
    }
    x = x << 8;
    x += std::stoi(temp);
    addr_i = x;
    return addr_i;
}
