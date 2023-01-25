#include <stdio.h>  // printf
#include <string.h> // strlen
#include <sys/socket.h> // socket
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>    // close

int main(int argc, char **argv) {
    int sock;
    struct sockaddr_in server;
    char move[10], server_reply[2000];
    printf("Adress: %s\n", argv[1]);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    // receive initial game board from server
    recv(sock, server_reply, sizeof server_reply, 0);
    puts(server_reply);

    while (1) {
        printf("Enter cell number (0-8) to make a move: ");
        scanf("%s", move);

        // Send move to server
        if (send(sock, move, strlen(move), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive updated game board or game result from server
        recv(sock, server_reply, sizeof(server_reply), 0);
        puts(server_reply);

        // check for win or draw
        if (strstr(server_reply, "wins") || strstr(server_reply, "draw")) {
            break;
        }
    }

    close(sock);
    return 0;
}