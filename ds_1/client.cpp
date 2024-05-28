#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

const std::string SERVER_IP = "217.113.2.231";
const int SERVER_PORT = 80;
const std::string FILENAME = "ysu.html";


int main(){
std::string req = "GET/main/HTTP/1.1\r\n";

req = req + "Hst: www.ysu.am\r\n";
req = req + "Connection: close\r\n\r\n";

int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket == -1){
		perror("socket");
		exit(errno);
	}

struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_port = htons(SERVER_PORT);
server_address.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

	if(connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1){
		perror("connect");
		
	if(close(client_socket) < 0){
		perror("close");
		exit(errno);
 	} 
	exit(errno);
	}

	if(send(client_socket, req.c_str(), req.length(), 0) == -1){
		perror("send");
	if(close(client_socket) < 0){
		perror("close");
		exit(errno);
	}
	exit(errno);
	}

std::stringstream r_buff;
char buffer[4096];
int b_received;

while((b_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0){
	r_buff.write(buffer, b_received);
}

	if(b_received == -1){
		perror("receive");
		if(close(client_socket) < 0){
			perror("close");
			exit(errno);
		}
	exit(errno);
	}

std::cout << r_buff.str();
std::ofstream outfile(FILENAME);
	if(!outfile){
		std::cerr<< "Cannot write";
		if(close(client_socket) < 0){
		perror("close");
		exit(errno);
		}
	exit(errno);
}

outfile<< r_buff.str();
outfile.close();

if(close(client_socket) < 0){
	perror("close");
	exit(errno);
}



return 0;
}
