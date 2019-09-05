#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "iMsg.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main(){
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
		return 0;

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(4444);
	
	for(int i = 0; i < 100000; i ++ ){
		string buff;
		iMsg pkg(i, 5, CLIENT_CONN_REQ, 2333, 0, 0);
		
		cout << "Please enter: " << endl;
		cin >> buff;
		int num = sendto(sock, buff.c_str(), strlen(buff.c_str()), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	}

	return 0;
}
