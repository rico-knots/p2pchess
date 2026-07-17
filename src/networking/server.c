// https://www.geeksforgeeks.org/c/socket-programming-cc/

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../chess.h"

#define PORT 8080

// Replace this the poll() eventually!
void *handle_client(void *arg) {
    int new_socket = *(int *) arg;
    free(arg);

    printf("\x1B[33m<server>\x1B[0m New connection: %d\n", new_socket);

    char buffer[1024];
    ssize_t valread;

    ChessBoard *board = malloc(sizeof(ChessBoard));

    // init_starting_position(board);
    print_board(board);

    while(1) {
        memset(buffer, 0, 1024 - 1);
        if ((valread = read(new_socket, buffer, 1024-1)) <= 0) {
            printf("\x1B[33m<server>\x1B[0m Client disconnected\n");
            break;
        }
        printf("\x1B[33m<server>\x1B[0m Echoing back: %s\n", buffer);
        printf("\x1B[33m<server>\x1B[0m Size of received on server: %ld\n", valread);
        send(new_socket, buffer, valread, 0);
    }
    close(new_socket);
    free(board);
    return NULL;
}

int main(int argc, char const* argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    char const* passcode = argv[1];
    if (passcode == NULL) {
        perror("Supply a passcode!\n");
        exit(EXIT_FAILURE);
        return 1;
    }
    
    printf("\x1B[33m<server>\x1B[0m Passcode: %s\n", passcode);

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