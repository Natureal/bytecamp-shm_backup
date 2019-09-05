#include "util/iChannel.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
using namespace std;

int main(){
	
	iAddr t_addr = iAddr("10.108.0.211");

	std::cout << t_addr.getAddr() << std::endl;
	iChannel tmp(t_addr);
	
	int ret = tmp.listen(10);
	
	cout << "listen ret: " << ret << endl;

	iChannel t2;
	
	while(tmp.accept(t2) == -1){

	}

	cout << "accept success" << endl;
	
	sleep(1);

	const int Size = 65536 * 2;

	char str[Size] = "Byte";
	int sum = 0;
	
	const int top = 1e8;

	//clock_t start, end;
	//start = clock();

	struct timeval t_start, t_end;
	gettimeofday(&t_start, NULL);

	while((ret = t2.write(str, Size, true)) >= 0){
		//if(ret == 0) continue;
		sum += ret;
		//cout << "Out: " << str << "Size: " << ret << " , sum: " << sum << endl;
		if(sum >= 1e8) break;
	}

	//end = clock();
	gettimeofday(&t_end, NULL);

	printf("size: %d\n", Size);
	double u_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0 + (t_end.tv_usec - t_start.tv_usec) / 1000.0;
	printf("sum: %dB, time: %f ms\n", sum, u_time);
	printf("throughout: %f MB/s\n", 1000.0 * sum / 1024.0 / 1024.0 / u_time);
	
	return 0;
}
