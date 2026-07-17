// https://www.geeksforgeeks.org/c/socket-programming-cc/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>

#include "packets.h"
#include "protocol.h"

#define PORT 8080

struct timespec ts = {0, 100 * 1000 * 1000}; // 100ms

pid_t start_server(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        execl("./out/server", "server", "password", (char *)NULL);
        perror("execl");
        exit(1);
    }

    return pid;
}

int connect_with_retry(const char *ip, int max_attempts) {
    struct sockaddr_in serv_addr;
    int status, client_fd;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // host to network short -> host byte order to network byte order

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) { // pton -> presentation to network ; reverse is inet_ntop(); -> network to presentation, binary to string representation
        printf(
            "\x1B[35m<client>\x1B[0m Invalid address/ Address not supported\n");
        return -1;
    }

    for (int i = 0; i < max_attempts; i++) {
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\x1B[35m<client>\x1B[0m Socket creation error \n");
            return -1;
        }

        if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == 0) {
            return client_fd;
        }

        close(client_fd);
        nanosleep(&ts, NULL);
    }
    return -1;
}

// Return the number read, -1 for errors, 0 for no read;
int read_socket(char buffer[], struct pollfd pfds[]) {
    int valread;
    memset(buffer, 0, 1024 - 1);

    if ((valread = poll(pfds, 1, 0)) < 0) {
        // Nothing read -> return nothing
        return -1;
    }

    if (pfds[0].revents & POLLIN) {
        // Event received
        if ((valread = read(pfds[0].fd, buffer, 1024 - 1)) <= 0 ) {
            printf("\x1B[35m<client>\x1B[0m Connection closed\n");
            return -1;
        }
    
        printf("\x1B[35m<client>\x1B[0m Received: %s\n", buffer);
        
        return valread;
    }

    if (pfds[0].revents & (POLLERR | POLLHUP)) {
        // Socket was closed
        printf("\x1B[35m<client>\x1B[0m Connection closed\n");
        return -1;
    }
    return 0;
}

int main(int argc, char const* argv[]) {
    int valread;
    struct pollfd pfds[1];
    char buffer[1024];

    ChessMovePacket move_packet = { 18, 26, 0, 30000 };
    ChessPacket packet = { PKT_MOVE };
    packet.move = move_packet;

    printf("\x1B[35m<client>\x1B[0m Packet type: %d\n", packet.type);

    if (argv[1] != NULL && strcmp(argv[1], "serv") == 0) {
        printf("\x1B[35m<client>\x1B[0m Starting server from client...\n");
        pid_t pid = start_server();
        if (pid < 0) return 1;
    }

    int client_fd = connect_with_retry("127.0.0.1", 20);
    
    if (client_fd < 0) {
        fprintf(stderr, "\x1B[35m<client>\x1B[0m Failed to connect to server\n");
        return 1;
    }

    pfds[0].fd = client_fd;
    pfds[0].events = POLLIN;
    
    int len;
    char *serialized = serialize_packet(&packet, &len);
    
    printf("\x1B[35m<client>\x1B[0m Serialized: %s\n", serialized);
    printf("\x1B[35m<client>\x1B[0m Serialize size: %d\n", len);
    
    // subtract 1 for the null terminator at the end
    int bytes_send = send(client_fd, serialized, len - 1, 0);
    free(serialized);

    nanosleep(&ts, NULL);

    int success = read_socket(buffer, pfds);

    // closing the connected socket
    close(client_fd);
    return 0;
}