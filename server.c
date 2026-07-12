// https://www.geeksforgeeks.org/c/socket-programming-cc/

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void *handle_client(void *arg) {
    int new_socket = *(int *) arg;
    free(arg);

    char buffer[1024] = { 0 };
    char *hello = "Hello from server";

    ssize_t valread = read(new_socket, buffer, 1024-1);
    printf("%s\n", buffer);

    send(new_socket, hello, strlen(hello), 0);
    printf("Hello msg sent\n");

    close(new_socket);
    return NULL;
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    char const* passcode = argv[1];
    if (passcode == NULL) {
        perror("Supply a passcode!\n");
        exit(EXIT_FAILURE);
        return 1;
    }
    
    printf("Passcode: %s\n", passcode);

    /*
        AF_INET = IPV4 | AF_LOCAL = LAN | AF_INET6 IPV6
        SOCK_STREAM = TCP connection | SOCK_DGRA = UDP connection
        protocol 0 = IP = internet protocol
    */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("getsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Handle incoming connections
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            continue;
        }

        // heap allocate a copy of the new socket
        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = new_socket;

        // Create thread
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, sock_ptr) != 0) {
            perror("pthread create");
            free(sock_ptr);
            close(new_socket);
            continue;
        }

        pthread_detach(tid);
    }
            
    // closing the listening socket
    close(server_fd);
    
    return 0;
}