// https://www.geeksforgeeks.org/c/socket-programming-cc/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

#include "packets.h"

#define PORT 8080

typedef struct {
    char name[32];
    char last_name[32];
} Person;

pid_t start_server(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        execl("./server", "server", "password", (char *)NULL);
        perror("execl");
        exit(1);
    }

    return pid;
}

int connect_with_retry(const char *ip, int max_attempts) {
    struct sockaddr_in serv_addr;
    int status, client_fd;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    for (int i = 0; i < max_attempts; i++) {
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            return -1;
        }

        if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == 0) {
            return client_fd;
        }

        close(client_fd);
        struct timespec ts = {0, 100 * 1000 * 1000}; // 100ms
        nanosleep(&ts, NULL);
    }
    return -1;
}

int main(int argc, char const* argv[])
{
    int valread;
    char buffer[1024] = { 0 };
    char* hello = "hello there";

    ChessResignPacket packet = { PKT_RESIGN };

    if (argv[1] != NULL && strcmp(argv[1], "serv") == 0) {
        printf("Starting server from client...");
        pid_t pid = start_server();
        if (pid < 0) return 1;
    }

    int client_fd = connect_with_retry("127.0.0.1", 20);
    
    if (client_fd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }
    
    // subtract 1 for the null terminator at the end
    int bytes_send = send(client_fd, &packet, sizeof(packet), 0);
    printf("Send: %d\n", packet.type);
    printf("Send bytes from client: %d\n", bytes_send);
    printf("Size of person: %ld\n", sizeof(Person));

    while(1) {
        bzero(buffer, 1024 - 1);
        
        if ((valread = read(client_fd, buffer, 1024 - 1)) <= 0 ) {
            printf("Connection closed\n");
            break;
        }
        printf("Received: %s\n", buffer);
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}