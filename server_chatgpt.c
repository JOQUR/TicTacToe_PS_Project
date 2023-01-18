#include <stdio.h>
#include <string.h> // strlen
#include <stdlib.h> // malloc
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr
#include <unistd.h> // write
#include <pthread.h> // pthread_create, pthread_join

#define MAX_CLIENTS 2
#define BOARD_SIZE 9

// game board
char board[BOARD_SIZE];

int client_count = 0;
pthread_mutex_t lock;
char message[2000];

// check for win
int check_win(char player) {
    if (board[0] == player && board[1] == player && board[2] == player) return 1;
    if (board[3] == player && board[4] == player && board[5] == player) return 1;
    if (board[6] == player && board[7] == player && board[8] == player) return 1;
    if (board[0] == player && board[3] == player && board[6] == player) return 1;
    if (board[1] == player && board[4] == player && board[7] == player) return 1;
    if (board[2] == player && board[5] == player && board[8] == player) return 1;
    if (board[0] == player && board[4] == player && board[8] == player) return 1;
    if (board[2] == player && board[4] == player && board[6] == player) return 1;
    return 0;
}

// check for draw
int check_draw() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] != 'X' && board[i] != 'O') return 0;
    }
    return 1;
}

// send game board to client
void send_board(int sock) {
    sprintf(message, " %c | %c | %c \n---+---+---\n %c | %c | %c \n---+---+---\n %c | %c | %c \n",
        board[0], board[1], board[2], board[3], board[4], board[5], board[6], board[7], board[8]);
    write(sock, message, strlen(message));
}

// connection handler for each client
void *connection_handler(void* socket_desc) {
    int sock = *(int*)socket_desc;
    int read_size, move;
    char player = (client_count == 1) ? 'X' : 'O';
    char message[2000];

    // send initial game board
    send_board(sock);

    while (1) {
        // receive move from client
        recv(sock, message, sizeof(message), 0);
        
sscanf(message, "%d", &move);

        // update board
        pthread_mutex_lock(&lock);
        if (board[move] == 'X' || board[move] == 'O') {
            sprintf(message, "Invalid move.\n");
            write(sock, message, strlen(message));
            pthread_mutex_unlock(&lock);
            continue;
        }
        board[move] = player;
        pthread_mutex_unlock(&lock);

        // send updated game board
        send_board(sock);

        // check for win or draw
        if (check_win(player)) {
            sprintf(message, "%c wins!\n", player);
            write(sock, message, strlen(message));
            break;
        } else if (check_draw()) {
            sprintf(message, "It's a draw.\n");
            write(sock, message, strlen(message));
            break;
        }
    }

    // close socket and decrease client count
    close(sock);
    pthread_mutex_lock(&lock);
    client_count--;
    pthread_mutex_unlock(&lock);

    return 0;
}

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c, *new_sock;
    struct sockaddr_in server, client;

    // initialize board
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i] = '0' + i;
    }

    // initialize mutex
    pthread_mutex_init(&lock, NULL);

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    // Listen
    listen(socket_desc, MAX_CLIENTS);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        pthread_mutex_lock(&lock);
        if (client_count >= MAX_CLIENTS) {
            sprintf(message, "Server is full.\n");
            write(client_sock, message, strlen(message));
            close(client_sock);
            pthread_mutex_unlock(&lock);
            continue;
        }
        client_count++;
        pthread_mutex_unlock(&lock);

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    // destroy mutex
    pthread_mutex_destroy(&lock);

    return 0;
}