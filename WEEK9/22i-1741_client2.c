#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3001

int main() {
    char message[256];
    int sock;
    struct sockaddr_in server_address;
    socklen_t server_address_len = sizeof(server_address);

    // Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    while (1) {
        printf("Enter a message: ");
        fgets(message, sizeof(message), stdin);
        // Remove newline character
        message[strcspn(message, "\n")] = '\0';

        // Send message to the server
        sendto(sock, message, strlen(message), 0, (struct sockaddr *) &server_address, server_address_len);

        // Exit if the message is "exit"
        if (strcmp(message, "exit") == 0)
            break;

        // Receive response from the server
        char response[256];
        ssize_t bytes_received = recvfrom(sock, response, sizeof(response), 0, (struct sockaddr *) &server_address, &server_address_len);
        
        response[bytes_received] = '\0';
        printf("Bytes received from server are : %zd\n", bytes_received);
        printf("Server response is : %s\n", response);
    }

    // Close the socket
    close(sock);

    return 0;
}
