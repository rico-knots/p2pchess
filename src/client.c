// https://www.geeksforgeeks.org/c/socket-programming-cc/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#define PORT 8080

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

    if (argv[1] != NULL && strcmp(argv[1], "serv") == 0) {
        printf("Starting server from client...");
        pid_t pid = start_server();
        if (pid < 0) return 1;
    }


    int client_fd = connect_with_retry("127.0.0.1", 20);
    
    // subtract 1 for the null
    // terminator at the end
    send(client_fd, hello, strlen(hello), 0);
    printf("Send: %s\n", hello);
    
    while(1) {
        valread = read(client_fd, buffer,
                   1024 - 1); 
        printf("Received: %s\n", buffer);
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}   