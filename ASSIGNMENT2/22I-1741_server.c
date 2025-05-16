#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void handle_student(int client_socket);
void handle_teacher(int client_socket);
void handle_client(int client_socket);

int main() {
    int server_socket, client_socket;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    pid_t pid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    listen(server_socket, MAX_CLIENTS);
    client_len = sizeof(client_addr);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("ERROR on accept");
            continue;
        }

        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            close(client_socket);
            continue;
        }

        if (pid == 0) { 
            close(server_socket);
            handle_client(client_socket);
            close(client_socket);
            exit(0);
        } else { 
            close(client_socket);
        }

        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    close(server_socket);
    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int n;

    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "Enter your role (teacher/student): ");
    n = write(client_socket, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        return;
    }

    bzero(buffer, BUFFER_SIZE);
    n = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        return;
    }

    buffer[n] = '\0';
    if (strncmp(buffer, "teacher", 7) == 0) {
        handle_teacher(client_socket);
    } else if (strncmp(buffer, "student", 7) == 0) {
        handle_student(client_socket);
    } else {
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, "Invalid role. Disconnecting...\n");
        write(client_socket, buffer, strlen(buffer));
    }
}

void handle_teacher(int client_socket) {
    char buffer[BUFFER_SIZE];
    char teacher_name[50];
    int n;

    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "Enter your name: ");
    n = write(client_socket, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        return;
    }

    bzero(buffer, BUFFER_SIZE);
    n = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        return;
    }

    buffer[n] = '\0';
    strcpy(teacher_name, buffer);

    FILE *file;
    file = fopen(teacher_name, "a"); 
    if (file == NULL) {
        perror("ERROR creating file");
        return;
    }
    fclose(file);

  while (1) {
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, "Enter command (add/view/exit): ");
        n = write(client_socket, buffer, strlen(buffer));
        if (n < 0) {
            perror("ERROR writing to socket");
            return;
        }

        bzero(buffer, BUFFER_SIZE);
        n = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            return;
        }

        buffer[n] = '\0';

        if (strncmp(buffer, "add", 3) == 0) {
            char student_info[BUFFER_SIZE];

            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Enter student info (roll number, marks): ");
            n = write(client_socket, buffer, strlen(buffer));
            if (n < 0) {
                perror("ERROR writing to socket");
                return;
            }

            bzero(buffer, BUFFER_SIZE);
            n = read(client_socket, buffer, BUFFER_SIZE - 1);
            if (n < 0) {
                perror("ERROR reading from socket");
                return;
            }

            buffer[n] = '\0';
            strcpy(student_info, buffer);

            file = fopen(teacher_name, "a");
            if (file == NULL) {
                perror("ERROR opening file");
                return;
            }

            fprintf(file, "%s\n", student_info);
            fclose(file);

            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Student info added successfully.\n");
            n = write(client_socket, buffer, strlen(buffer));
            if (n < 0) {
                perror("ERROR writing to socket");
                return;
            }

        } else if (strncmp(buffer, "view", 4) == 0) {
            // View student information
            file = fopen(teacher_name, "r");
            if (file == NULL) {
                perror("ERROR opening file");
                return;
            }

            bzero(buffer, BUFFER_SIZE);
            while (fgets(buffer, BUFFER_SIZE - 1, file) != NULL) {
                n = write(client_socket, buffer, strlen(buffer));
                if (n < 0) {
                    perror("ERROR writing to socket");
                    fclose(file);
                    return;
                }
                bzero(buffer, BUFFER_SIZE);
            }
            fclose(file);

            strcpy(buffer, "End of student information.\n");
            write(client_socket, buffer, strlen(buffer));

        } else if (strncmp(buffer, "exit", 4) == 0) {
            break;
        } else {
            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, "Invalid command.\n");
            n = write(client_socket, buffer, strlen(buffer));
            if (n < 0) {
                perror("ERROR writing to socket");
                return;
            }
        }
    }
}

void handle_student(int client_socket) {
    char buffer[BUFFER_SIZE];
    char roll_number[50];
    int n;

    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "Enter your roll number: ");
    n = write(client_socket, buffer, strlen(buffer));
    if (n < 0) {
        perror("ERROR writing to socket");
        return;
    }

    bzero(buffer, BUFFER_SIZE);
    n = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("ERROR reading from socket");
        return;
    }

    buffer[n] = '\0';
    strcpy(roll_number, buffer);

    char line[BUFFER_SIZE];
    FILE *file;
    int found = 0;

    for (int i = 0; i < 10; i++) { 
        char teacher_name[50];
        sprintf(teacher_name, "teacher%d", i+1); 
        file = fopen(teacher_name, "r");
        if (file == NULL) {
            continue;
        }

        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, roll_number) != NULL) {
                found = 1;
                write(client_socket, line, strlen(line));
            }
        }

        fclose(file);
    }

    if (!found) {
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, "Roll number not found.\n");
        write(client_socket, buffer, strlen(buffer));
    }
}
