#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread

//the thread function
void *connection_handler(void *);

int check_win(char game_board[]) {
    // check rows
    for (int i = 0; i < 9; i += 3) {
        if (game_board[i] != ' ' && game_board[i] == game_board[i + 1] && game_board[i] == game_board[i + 2]) {
            return 1;
        }
    }
    // check columns
    for (int i = 0; i < 3; i++) {
        if (game_board[i] != ' ' && game_board[i] == game_board[i + 3] && game_board[i] == game_board[i + 6]) {
            return 1;
        }
    }
    // check diagonals
    if (game_board[0] != ' ' && game_board[0] == game_board[4] && game_board[0] == game_board[8]) {
        return 1;
    }
    if (game_board[2] != ' ' && game_board[2] == game_board[4] && game_board[2] == game_board[6]) {
        return 1;
    }
    return 0;
}

int check_draw(char game_board[]) {
    for (int i = 0; i < 9; i++) {
        if (game_board[i] == ' ') {
            return 0;
        }
    }
    return 1;
}


int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void* socket_desc) {
    int sock = (int)socket_desc;
    char client_message[2000], game_board[9] = {' ',' ',' ',' ',' ',' ',' ',' ',' '};
    int read_size, player = 1;
    // Send a message to the client to start the game
    write(sock, "Welcome to Tic-Tac-Toe! You are player 1.\n", strlen("Welcome to Tic-Tac-Toe! You are player 1.\n"));
    while(1) {
        // Receive a move from the client
        read_size = recv(sock , client_message , 2000 , 0);
        if(read_size <= 0) {
            puts("Client disconnected");
            break;
        }
        int move = atoi(client_message);
        if (move < 0 || move > 8 || game_board[move] != ' ') {
            write(sock, "Invalid move, try again\n", strlen("Invalid move, try again\n"));
            continue;
        }
        game_board[move] = player == 1 ? 'X' : 'O';
        // Send the updated game board to the client
        write(sock , game_board , sizeof(game_board));
        if (check_win(game_board)) {
            char* win_message = player == 1 ? "Player 1 wins!\n" : "Player 2 wins!\n";
            write(sock, win_message, strlen(win_message));
            break;
        }
        if (check_draw(game_board)) {
            write(sock, "It's a draw!\n", strlen("It's a draw!\n"));
            break;
        }
        player = player == 1 ? 2 : 1;
    }
    free(socket_desc);
    return 0;
}
