
/*Include Section*/
#include   <sys/types.h>   /* basic system data types */
#include   <sys/socket.h>  /* basic socket definitions */
#include   <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include   <arpa/inet.h>   /* inet(3) functions */
#include   <errno.h>
#include   <stdio.h>
#include   <stdlib.h>
#include   <unistd.h>
#include   <string.h>
/*Include Section*/

/*Define Section*/
#define MAXLINE 1024
#define SA      struct sockaddr
/*Define Section*/



int
main(int argc, char **argv)
{	
	char buff[MAXLINE];
	char text[MAXLINE];
	int	sockfd, n, err;
	struct sockaddr_in	servaddr;
	char recvline[MAXLINE + 1];

	printf("What you want to send?\n");


	if (argc != 2){
		fprintf(stderr, "usage: %s <IPaddress> \n", argv[0]);
		return 1;
	}

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr,"socket error : %s\n", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if ( (err=inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) == -1){
		fprintf(stderr,"ERROR: inet_pton error for %s : %s \n", argv[1], strerror(errno));
		return 1;
	}else if(err == 0){
		fprintf(stderr,"ERROR: Invalid address family \n");
		return 1;
	}
	
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		fprintf(stderr,"connect error : %s \n", strerror(errno));
		return 1;
	}

	while(1){
        
	    scanf("%s", text);
        snprintf(buff, sizeof(buff), "%s\n", text);
        if( write(sockfd, buff, strlen(buff))< 0 )
            fprintf(stderr,"write error : %s\n", strerror(errno));

        
    }

	fflush(stdout);
	fgetc(stdin);	
	exit(0);
}

