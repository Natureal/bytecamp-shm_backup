#include "iChannel.h"
#include <string>
#include "iAddr.h"

using namespace std;

int main() {
    string ip = "10.109.0.132";
    iChannel my(iAddr(ip));
    string serverip = "10.108.0.211";
    my.connect(iAddr(serverip));

    return 0;
}