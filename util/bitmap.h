#ifndef BITMAP_H
#define BITMAP_H

#include <vector>

class BitMap {
public:
    BitMap(int num=0):n(num),mask(0x1F),shift(5),pos(1<<mask),a(1+n/32,0){}
    void set(int i) {
        a[i>>shift] |= (pos>>(i & mask));
    }
    int test(int i) {
        return a[i>>shift] & (pos>>(i & mask));
    }
    void clear(int i) {
        a[i>>shift] &= ~(pos>>(i & mask));     
    }   
private:
    int n;
    const int mask;
    const int shift;
    const unsigned int pos;
    std::vector<unsigned int> a;
};

#endif
