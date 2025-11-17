#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(){
	// return file descrepter of the socket
	int s = socket(AF_INET,SOCK_STREAM,0); 
	printf("Socket s:%d\n\n",s);
	printf("Another socket FD: %d\n", socket(AF_INET, SOCK_STREAM, 0)); // Will print 4
									    
	struct sockaddr_in addr = {
		AF_INET,
		0x901f,
		0
	};

	// takes the socket s : 3
	//  bind using IPv4, port 8080, sizeof
	bind(s, &addr, sizeof(addr)); 
	listen(s, 10);

	// return file descrepter of the client socket
	int client_fd = accept(s, 0, 0);
	printf("Client fd:%d\n\n",client_fd);

	// get the http request GET
	char buffer[256] = {0};
	recv(client_fd, buffer, 256, 0);
	printf("Buffer:[%s]\n\n",buffer);

	// GET /file.html ...
	char* f = buffer + 5;

	*strchr(f, ' ') = 0; // change space to null terminator
	int opened_fd = open(f, O_RDONLY);
	printf("File f:%s\n\n",f);

	printf("Socket s:%d\n\n",s);

	printf("Opened_fd:%d\n\n",opened_fd);

	// transfer data btw file desc
	sendfile(client_fd, opened_fd, 0, 256);
	close(opened_fd);
	close(client_fd);
	close(s);
}
