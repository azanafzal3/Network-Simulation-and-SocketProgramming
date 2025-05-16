#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_teacher(int sockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    // Enter teacher name
    printf("Enter your name: ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE-1, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }


    while (1) {
        printf("Enter command (add/view/exit): ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE-1, stdin);
        buffer[strcspn(buffer, "\n")] = 0; 

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        bzero(buffer, BUFFER_SIZE);
        n = read(sockfd, buffer, BUFFER_SIZE-1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("%s\n", buffer);

        if (strncmp(buffer, "Enter student info", 18) == 0) {
            bzero(buffer, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE-1, stdin);
            buffer[strcspn(buffer, "\n")] = 0; 

            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                perror("ERROR writing to socket");
                exit(1);
            }
        }
    }
}

void handle_student(int sockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    printf("Enter your roll number: ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE-1, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("%s\n", buffer);
}

void communicate_with_server(int sockfd) {
    char buffer[BUFFER_SIZE];
    int n;

    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("%s", buffer);

    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE-1, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    if (strncmp(buffer, "teacher", 7) == 0) {
        handle_teacher(sockfd);
    } else if (strncmp(buffer, "student", 7) == 0) {
        handle_student(sockfd);
    } else {
        printf("Invalid role. Disconnecting...\n");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    communicate_with_server(sockfd);

    close(sockfd);
    return 0;
}
