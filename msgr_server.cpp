#include "util/msgr.h"

int main(){
	msgr* manager;
	try{
		manager = new msgr;
	}catch(...){
		perror("Failed.");
	}
	cout << "Manager starts running..." << endl;
	manager->run();
	
	delete manager;
	return 0;
}
