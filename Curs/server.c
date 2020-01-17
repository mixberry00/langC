#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h> 	/* for threads */
#include <sys/msg.h>
#include <sys/errno.h>

#define UDPcount 10
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define MAXCLNTSTRLEN 100

int msgqid, rc;

struct mymsgbuf 
{
    long mtype;
    int mT;
    int mN;
    char mtext[MAXCLNTSTRLEN];
};

/* Structure of arguments to pass to client thread */
struct ThreadArgs
{
    int *UDPsockets;                      /* Socket descriptor for client */
    struct sockaddr_in *broadcastAddr;
};

struct ThreadArgsTCP
{
    int TCPsocket;                      /* Socket descriptor for client */
    struct sockaddr_in TCPAddr;
};

struct mymsg 
{
    int T;
    int N;
};

void DieWithError(char *errorMessage);  /* External error handling function */

void send_message(int qid, struct mymsgbuf *qbuf, long type, int T, int N, char *text);

char *read_message(int qid, struct mymsgbuf *qbuf, long type);

void *UDPgetThread(void *arg);

void *UDPsendThread(void *arg);

void *TCPgetThread(void *arg);

void *TCPsendThread(void *arg);

int CreateTCPServerSocket(unsigned short port, struct sockaddr_in *TCPServAddr);

int main(int argc, char *argv[])
{
	//Threads param
    int result;
    void *status[4];
    pthread_t tid[4];
    //TCP param
    int TCPservSockF, TCPservSockS;
    int TCPsockF, TCPsockS;
    struct sockaddr_in TCPServAddrF, TCPServAddrS;
    unsigned short TCPServPortF = 32000, TCPServPortS = 32001;     	
    struct ThreadArgsTCP *threadArgsTCPget, *threadArgsTCPsend;

    //UDP param
    int sockF[UDPcount], sockS[UDPcount];                         			/* Socket */
    struct sockaddr_in broadcastAddrF[UDPcount], broadcastAddrS[UDPcount];  /* Broadcast address */
    char *broadcastIP;                										/* IP broadcast address */
    unsigned short broadcastPortF[UDPcount], broadcastPortS[UDPcount];     	/* Server port */
    int broadcastPermission = 1;          									/* Socket opt to set permission to broadcast */
    struct ThreadArgs *threadArgsUDPget, *threadArgsUDPsend;   				/* Pointer to argument structure for thread */
    
    //Create queue ident    
    if((msgqid = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1) {
       perror("msgget");
       exit(1);
    }

    //broadcast IP address
    broadcastIP = "127.0.0.1";
    //broadcast ports for get
    for(int i = 32002, j = 0; i < 32011; i++)
        broadcastPortF[j++] = i;
    //broadcast ports for send
    for(int i = 32014, j = 0; i < 32024; i++)
        broadcastPortS[j++] = i;

    for(int i = 0; i < UDPcount; i++)
    {
        /* Create socket for sending/receiving datagrams */
        if ((sockF[i] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            DieWithError("socket() failed");

        /* Create socket for sending/receiving datagrams */
        if ((sockS[i] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            DieWithError("socket() failed");

        /* Set socket to allow broadcast */
        if (setsockopt(sockF[i], SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
                       sizeof(broadcastPermission)) < 0)
            DieWithError("setsockopt() failed");

        if (setsockopt(sockS[i], SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
                       sizeof(broadcastPermission)) < 0)
            DieWithError("setsockopt() failed");

        /* Construct local address structure */
        memset(&broadcastAddrF[i], 0, sizeof(broadcastAddrF[i]));   /* Zero out structure */
        broadcastAddrF[i].sin_family = AF_INET;                 /* Internet address family */
        broadcastAddrF[i].sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
        broadcastAddrF[i].sin_port = htons(broadcastPortF[i]);         /* Broadcast port */

        memset(&broadcastAddrS[i], 0, sizeof(broadcastAddrS[i]));   /* Zero out structure */
        broadcastAddrS[i].sin_family = AF_INET;                 /* Internet address family */
        broadcastAddrS[i].sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
        broadcastAddrS[i].sin_port = htons(broadcastPortS[i]);         /* Broadcast port */
    }

    //Create UDPGET Thread args
    /* Create separate memory for client argument */
    if ((threadArgsUDPget = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs)))
            == NULL)
        DieWithError("malloc() failed");
    threadArgsUDPget -> UDPsockets = sockF;
    threadArgsUDPget -> broadcastAddr = broadcastAddrF;
    
    //Create UDPSEND Thread args
    /* Create separate memory for client argument */
    if ((threadArgsUDPsend = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs)))
            == NULL)
        DieWithError("malloc() failed");
    threadArgsUDPsend -> UDPsockets = sockS;
    threadArgsUDPsend -> broadcastAddr = broadcastAddrS;

    //Create TCPget sock
    TCPservSockF = CreateTCPServerSocket(TCPServPortF, &TCPServAddrF);

    //Create TCPget thread args
    /* Create separate memory for client argument */
    if ((threadArgsTCPget = (struct ThreadArgsTCP *) malloc(sizeof(struct ThreadArgsTCP)))
            == NULL)
        DieWithError("malloc() failed");
    threadArgsTCPget -> TCPsocket = TCPservSockF;
    threadArgsTCPget -> TCPAddr = TCPServAddrF;
    
    //Create TCPsend sock
    TCPservSockS = CreateTCPServerSocket(TCPServPortS, &TCPServAddrS);

    //Create TCPsend thread args
    /* Create separate memory for client argument */
    if ((threadArgsTCPsend = (struct ThreadArgsTCP *) malloc(sizeof(struct ThreadArgsTCP)))
            == NULL)
        DieWithError("malloc() failed");
    threadArgsTCPsend -> TCPsocket = TCPservSockS;
    threadArgsTCPsend -> TCPAddr = TCPServAddrS;

    printf("Server is working\n");

    /* Create UDPget thread */
    if (pthread_create(&tid[0], NULL, UDPgetThread, (void *) threadArgsUDPget) != 0)
        DieWithError("pthread_create() failed");
        
    /* Create UDPsend thread */
    if (pthread_create(&tid[1], NULL, UDPsendThread, (void *) threadArgsUDPsend) != 0)
        DieWithError("pthread_create() failed");
        
    /* Create TCPget thread */
    if (pthread_create(&tid[2], NULL, TCPgetThread, (void *) threadArgsTCPget) != 0)
        DieWithError("pthread_create() failed");
        
    /* Create TCPsend thread */
    if (pthread_create(&tid[3], NULL, TCPsendThread, (void *) threadArgsTCPsend) != 0)
        DieWithError("pthread_create() failed");
        
	//wait ending of each thread
    for(int i = 0; i < 4; i++)
    {
        result = pthread_join(tid[i], &status[i]);
        if (result != 0) {
            perror("Joining the first thread");
            return EXIT_FAILURE;
        }
        free(status[i]);
        close(TCPservSockF);
        close(TCPservSockS);
    }
    
    if ((rc = msgctl(msgqid, IPC_RMID, NULL)) < 0) {
        perror( strerror(errno) );
        printf("msgctl (return queue) failed, rc=%d\n", rc);
        return 1;
    }
    
    return 0;
}

void send_message(int qid, struct mymsgbuf *qbuf, long type, int T, int N, char *text) {
    qbuf->mtype = type;
    qbuf->mT = T;
    qbuf->mN = N;
    strcpy(qbuf->mtext, text);
    if((msgsnd(qid, (struct msgbuf *)qbuf,
               sizeof(struct mymsgbuf) - sizeof(long), 0)) ==-1) {
        perror("msgsnd");
        exit(1);
    }
}

char *read_message(int qid, struct mymsgbuf *qbuf, long type) {
    qbuf->mtype = type;
    msgrcv(qid, (struct msgbuf *)qbuf, sizeof(struct mymsgbuf) - sizeof(long), type, 0);
    return qbuf->mtext;
}


int CreateTCPServerSocket(unsigned short port, struct sockaddr_in *TCPServAddr)
{
    int sock;                        /* socket to create */

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(TCPServAddr, 0, sizeof(TCPServAddr));   /* Zero out structure */
    TCPServAddr->sin_family = AF_INET;                /* Internet address family */
    TCPServAddr->sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    TCPServAddr->sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) TCPServAddr, sizeof(*TCPServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(sock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    return sock;
}

void *UDPgetThread(void *threadArgs)
{
    int *sockF = ((struct ThreadArgs *) threadArgs) -> UDPsockets;
    char *sendString = "I'm ready to get msg";
    int sendStringLen = strlen(sendString);  /* Find length of sendString */
    struct sockaddr_in *broadcastAddrF = ((struct ThreadArgs *) threadArgs) -> broadcastAddr;
    for(;;)
    {
        for(int i = 0; i < UDPcount - 1; i++)
        {
            /* Broadcast sendString in datagram to clients every 5 seconds*/
            if (sendto(sockF[i], sendString, sendStringLen, 0, (struct sockaddr *)
                       &broadcastAddrF[i], sizeof(broadcastAddrF[i])) != sendStringLen)
                DieWithError("sendto() sent a different number of bytes than expected");
        }
        sleep(5);
    }
}

void *UDPsendThread(void *threadArgs)
{
	int *sockS = ((struct ThreadArgs *) threadArgs) -> UDPsockets;
    char *sendString = "I'm ready to send msg";
    int sendStringLen = strlen(sendString);  /* Find length of sendString */
    struct sockaddr_in *broadcastAddrF = ((struct ThreadArgs *) threadArgs) -> broadcastAddr;
    for(;;)
    {
        for(int i = 0; i < UDPcount - 1; i++)
        {
            /* Broadcast sendString in datagram to clients every 5 seconds*/
            if (sendto(sockS[i], sendString, sendStringLen, 0, (struct sockaddr *)
                       &broadcastAddrF[i], sizeof(broadcastAddrF[i])) != sendStringLen)
                DieWithError("sendto() sent a different number of bytes than expected");
        }

        sleep(5);
    }
}

void *TCPgetThread(void *threadArgs)
{
	int TCPsockClnt;
	char clntstr[MAXCLNTSTRLEN];
	struct mymsgbuf qbuf;
	int qtype = 1;
	int TCPservSock = ((struct ThreadArgsTCP *) threadArgs) -> TCPsocket;
	struct sockaddr_in TCPclntAddr = ((struct ThreadArgsTCP *) threadArgs) -> TCPAddr;
	/* Set the size of the in-out parameter */
    int clntLen = sizeof(TCPclntAddr);
    /* Guarantees that thread resources are deallocated upon return */
    pthread_detach(pthread_self()); 
    for(;;)
    {
		/* Wait for a client to connect */
		if ((TCPsockClnt = accept(TCPservSock, (struct sockaddr *) &TCPclntAddr,
							   &clntLen)) < 0)
			DieWithError("accept() failed");

		/* clntSock is connected to a client! */

		printf("Handling client %s\n", inet_ntoa(TCPclntAddr.sin_addr));

		int recvMsgSize;

		struct mymsg msg;

		if ((recvMsgSize = recv(TCPsockClnt, &msg, sizeof(msg), 0)) < 0)
			DieWithError("recv() failed");		
		
		if ((recvMsgSize = recv(TCPsockClnt, clntstr, msg.N, 0)) < 0)
			DieWithError("recv() failed");
			
		clntstr[msg.N] = '\0';
		
		send_message(msgqid, (struct mymsgbuf *)&qbuf, qtype, msg.T, msg.N, clntstr);
		
		close(TCPsockClnt);    /* Close client socket */

		sleep(3);   /* Control server from overpower */
	}
}

void *TCPsendThread(void *threadArgs)
{
	int TCPsockClnt;
	char clntstr[MAXCLNTSTRLEN];
	struct mymsgbuf qbuf;
	int qtype = 1;
	int TCPservSock = ((struct ThreadArgsTCP *) threadArgs) -> TCPsocket;
	struct sockaddr_in TCPclntAddr = ((struct ThreadArgsTCP *) threadArgs) -> TCPAddr;
	/* Set the size of the in-out parameter */
    int clntLen = sizeof(TCPclntAddr);
    //Client number
    int T;
    /* Guarantees that thread resources are deallocated upon return */
    pthread_detach(pthread_self()); 
    for(;;)
    {
		/* Wait for a client to connect */
		if ((TCPsockClnt = accept(TCPservSock, (struct sockaddr *) &TCPclntAddr,
							   &clntLen)) < 0)
			DieWithError("accept() failed");

		/* clntSock is connected to a client! */

		printf("Handling client %s\n", inet_ntoa(TCPclntAddr.sin_addr));
		
		int recvMsgSize;
		
		if ((recvMsgSize = recv(TCPsockClnt, &T, sizeof(int), 0)) < 0)
			DieWithError("recv() failed");
			
		printf("Client number: %d\n", T);
		
		char *recvstr = read_message(msgqid, (struct mymsgbuf *)&qbuf, qtype);
		
		int sendMsgSize = strlen(recvstr);

		/* Echo message back to client */
        if (send(TCPsockClnt, recvstr, sendMsgSize, 0) != sendMsgSize)
            DieWithError("send() failed");
            
        printf("Message sent\n");
		
		close(TCPsockClnt);    /* Close client socket */

		sleep(3);   /* Control server from overpower */
	}
}
