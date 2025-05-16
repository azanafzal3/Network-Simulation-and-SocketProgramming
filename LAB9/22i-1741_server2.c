#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT 3001

void handle_client(int server_socket);

int main() {
    int server_socket;
    struct sockaddr_in server_address;

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Listening on port %d ...\n", SERVER_PORT);

    // Handle client messages
    handle_client(server_socket);

    // Close the server socket
    close(server_socket);

    return 0;
}

void handle_client(int server_socket) {
    char buf[256];
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    while (1) {
        // Receive data from the client
        ssize_t bytes_received = recvfrom(server_socket, buf, sizeof(buf), 0, (struct sockaddr *) &client_address, &client_address_len);
        
        buf[bytes_received] = '\0';
        printf("received bytes from client side is : %zd\n", bytes_received);
        printf("Client message is : %s\n", buf);

        // Exit if the message is "exit"
        if (strcmp(buf, "exit") == 0)
            break;

        // Send response to the client
        char response[] = "Message received";
        sendto(server_socket, response, strlen(response), 0, (struct sockaddr *) &client_address, client_address_len);
    }
}
