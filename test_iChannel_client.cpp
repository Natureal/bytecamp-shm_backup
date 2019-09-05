#include "util/iChannel.h"
#include <string.h>
#include <sys/time.h>

using namespace std;

int main(){

	iAddr t_addr = iAddr("10.108.0.212");
	iAddr server_addr = iAddr("10.108.0.211");
	
	iChannel tmp(t_addr);

	int ret;
	
	while((ret = tmp.connect(server_addr)) < 0){
		cout << "connect ret: " << ret << endl;
	}

	const int Size = 65536 * 2;
	
	char str[Size];

	int sum = 0;

	sleep(1);

	//clock_t start, end;
	//start = clock();

	struct timeval t_start, t_end;
	gettimeofday(&t_start, NULL);
	const int top = 1e8;

	while((ret = tmp.read(str, Size, false)) >= 0){
		//if(ret == 0) continue;
		sum += ret;
		//cout << "success Size: " << ret << " , sum: " << sum << endl;
		if(sum >= top) break;
	}
	
	//end = clock();
	gettimeofday(&t_end, NULL);
	//long used_time = (t_end.tv_sec - t_start.tv_sec) * 1000 + (t_end.tv_usec - t_start.tv_usec) / 1000;
	//printf("%ld\n", used_time);
	//printf("sum: %d, CPU_time: %f\n", sum, (double)(end - start) / CLOCKS_PER_SEC * 1000.0);
	double u_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0 + (t_end.tv_usec - t_start.tv_usec) / 1000.0;
	
	printf("size: %d\n", Size);
	printf("sum: %d, Absolute_tim: %f\n", sum, u_time);
	printf("throughout: %f MB/s\n", 1000.0 * sum / 1024.0 / 1024.0 / u_time);
	return 0;
}

