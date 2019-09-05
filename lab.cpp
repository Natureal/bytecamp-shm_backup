#include <string>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(){
	cout << to_string(syscall(SYS_gettid)) << endl;
	return 0;
}
