#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define SERVER_IP "127.0.0.1" // Change this to the IP address of the server
int main() {
char request[256];
int num1, num2, result;
char buf[200];
// create the socket
int sock;
sock = socket(AF_INET, SOCK_DGRAM, 0);
// setup an address
struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
server_address.sin_port = htons(3001);
// Send the first message (numbers) to the server
printf("Enter a number: ");
scanf("%d", &num1);

sprintf(request, "%d ", num1);
sendto(sock, request, strlen(request), 0, (struct sockaddr *) &server_address,
sizeof(server_address));

// close the socket

close(sock);
return 0;
}
