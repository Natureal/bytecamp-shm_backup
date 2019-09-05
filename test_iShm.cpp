#include "util/iShm.h"
#include <cstdint>
#include <iostream>
using namespace std;

char str[1056];

int main(){
	
	iShm tmp(str, 4096 * 32);
	
	char *pBuffer = tmp.pBuffer;
	
	cout << tmp.server_lens << endl;
	cout << tmp.client_lens << endl;

	char write_str[64] = "Hello World";

	for(int i = 1; i <= 100; ++i){

		int c1 = tmp.shm_write((void*)write_str, 20, true);

		char read_str[20];

		int c2 = tmp.shm_read((void*)read_str, 20, false);

		cout << "c1 : " << c1 << endl;
		cout << "c2 : " << c2 << " string: " << read_str << endl;
		
		cout << "server_writeCursor: " << tmp.char2uint(server_writeCursor) << endl;
		cout << "server_readCursor: " << tmp.char2uint(server_readCursor) << endl;
		cout << "Cliet_writeCursor: " << tmp.char2uint(client_writeCursor) << endl;
		cout << "Client_readCursor: " << tmp.char2uint(client_readCursor) << endl;


	}

	

	return 0;
}
