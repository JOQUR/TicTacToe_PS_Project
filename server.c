#include        <sys/types.h>   
#include        <sys/socket.h>  
#include        <netinet/in.h>  
#include        <arpa/inet.h>   
#include        <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	    <unistd.h>

#define MAX_BUFF 1024
#define LISTENQ 2

int main(int argc, char** argv){

    int listenfd, connfd;
    socklen_t len;
    char buff[MAX_BUFF], str[INET_ADDRSTRLEN + 1];
    time_t ticks;
    struct sockaddr_in servaddr, cliaddr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);
    if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
            fprintf(stderr,"bind error : %s\n", strerror(errno));
            return 1;
        }

    if ( listen(listenfd, LISTENQ) < 0){
        fprintf(stderr,"listen error : %s\n", strerror(errno));
        return 1;
    }

    while(1){
        len = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len)) < 0){
            fprintf(stderr,"accept error : %s\n", strerror(errno));
            continue;
        }

        bzero(str, sizeof(str));
        inet_ntop(AF_INET, (struct sockaddr  *) &cliaddr.sin_addr.s_addr,  str, sizeof(str));
        printf("Connection from %s\n", str);
       
        
    }
        close(connfd);
    

    return 0;
}