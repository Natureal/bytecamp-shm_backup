#include "util/bitmap.h"
#include <iostream>

using namespace std;

void TestBitMap()
{
    BitMap bm(33);
    cout << bm.test(174850259) << endl;
    cout << bm.test(8) << endl;
    bm.set(87999);
    bm.clear(87999);
    cout << bm.test(87999) << endl;
    cout << bm.test(87777) << endl;
    
}

int main()
{
    TestBitMap();
    return 0;
}
