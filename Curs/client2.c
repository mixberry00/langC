#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXRECVSTRING 255  /* Longest string to receive */
#define MAXCLNTSTRLEN 100

struct mymsg{
	int T;
	int N;
};

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
	//TCP param
	int TCPsock;                        		/* Socket descriptor */
    struct sockaddr_in TCPServAddr; 		/* Echo server address */
    unsigned short TCPServPort = 32001;     /* Echo server port */
    char *servIP = "127.0.0.1";                    		/* Server IP address (dotted quad) */
    struct mymsg msg;
    char servstring[MAXCLNTSTRLEN+1];
    int servstringlen;
    int T;
    
    //UDP param
    int UDPsock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    unsigned short broadcastPort;     /* Port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */
    
    if (argc != 2)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Broadcast Port>\n", argv[0]);
        exit(1);
    }

    broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */
    
    srand(getpid());

    /* Create a best-effort datagram socket using UDP */
    if ((UDPsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(UDPsock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        DieWithError("bind() failed");

    /* Receive a single datagram from the server */
    if ((recvStringLen = recvfrom(UDPsock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
        DieWithError("recvfrom() failed");

    recvString[recvStringLen] = '\0';
    printf("Received: %s\n", recvString);    /* Print the received string */     
    
    //Send msg by TCP
    /* Create a reliable, stream socket using TCP */
    if ((TCPsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&TCPServAddr, 0, sizeof(TCPServAddr));     /* Zero out structure */
    TCPServAddr.sin_family      = AF_INET;             /* Internet address family */
    TCPServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    TCPServAddr.sin_port        = htons(TCPServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(TCPsock, (struct sockaddr *) &TCPServAddr, sizeof(TCPServAddr)) < 0)
        DieWithError("connect() failed");
        
    T = rand() % 50;
    
    printf("Your number is: %d\n", T);
    
    if (send(TCPsock, &T, sizeof(int), 0) != sizeof(int))
      DieWithError("send() sent a different number of bytes than expected");
        
    /* Receive the same string back from the server */
    int bytesRcvd;
    printf("Received: ");                /* Setup to print the echoed string */
    /* Receive up to the buffer size (minus 1 to leave space for
       a null terminator) bytes from the sender */
    if ((bytesRcvd = recv(TCPsock, servstring, MAXCLNTSTRLEN - 1, 0)) <= 0)
        DieWithError("recv() failed or connection closed prematurely");
    servstring[bytesRcvd] = '\0';  /* Terminate the string! */
    printf("%s", servstring);      /* Print the echo buffer */

    printf("\n");    /* Print a final linefeed */       
    
    close(UDPsock);
    close(TCPsock);
    exit(0);
}
