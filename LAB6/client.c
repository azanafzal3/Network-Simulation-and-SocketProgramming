#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main() 
{
char request[256] = "hello from the client side... !";
char buf[200];
// create the socket
int sock;
sock = socket(AF_INET, SOCK_STREAM, 0);
//setup an address
struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(3001);
connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));

while(1)
{
    char arr[50];
    printf("Enter String : ");
    scanf("%s",&arr);
    if(arr[0]=='0')
        break;
    send(sock, arr, sizeof(request), 0);
    recv(sock, &buf, sizeof(buf), 0);
}

printf("\n %s \n",buf);

close(sock);
return 0;
}
