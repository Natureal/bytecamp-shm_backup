#include "util/aQueue.h"
#include <iostream>
#include <cstdint>

void pop(aQueue<uint64_t>&q, int n) {
    for(int i = 0; i < n; i ++ ){
	    uint64_t tmp;
	    if (q.dequeue(tmp)) std::cout << tmp << std::endl;
        else std::cout << "full" << std::endl;
    }
}

int main() {
    int n = 5;
    int size = 6 * n * 64 + 128;
    void* pt = malloc(size);
    memset(pt, 0, size);
    aQueue<uint64_t> q(pt, 7);
    for(uint64_t i = 0; i < 8; i ++ ){
	    q.enqueue(i);
    }

    pop(q, 3);
    for(uint64_t i = 0; i < 4; i ++ ){
        uint64_t t = i + 100;
        q.enqueue(t);
    }

    pop(q, 5);

    

    return 0;
}
