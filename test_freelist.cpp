#include "util/freeList.h"
#include <iostream>

using namespace std;

int main() {
    freeList fl;
    int n = 30;
    for(int i = 0; i < n; i ++ ){
	int t = fl.getFreeFd();
    }

    return 0;
}
