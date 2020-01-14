#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h> 	/* for threads */

#define UDPcount 10
#define MAXPENDING 5    /* Maximum outstanding connection requests */

static int ntr = 3;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

void DieWithError(char *errorMessage);  /* External error handling function */

struct mymsg {
    int T;
    int N;
    char *text;
};

void* echo( void* ps ) {
    int sc = *(int*)ps, rs;
    sched_yield();
    if( ( rs = accept( sc, NULL, NULL ) ) < 0 ) errx( "accept error" );
    retrans( rs );
    close( rs );
    pthread_mutex_lock( &mutex );
    ntr++;
    pthread_cond_signal( &condvar );
    pthread_mutex_unlock( &mutex );
    if( debug ) cout < pthread_self() < '.' < flush;
    delay( 250 );  // пауза 250 usec.
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    //TCP param
    int TCPservSock;
    int TCPsockF, TCPsockS;
    struct sockaddr_in TCPServAddr, TCPclntAddrF;
    unsigned short TCPServPortF = 32000;     	/* Server port */
    unsigned int clntLen;            			/* Length of client address data structure */
    struct mymsg *msg;

    //UDP param
    int sockF[UDPcount], sockS[UDPcount];                         			/* Socket */
    struct sockaddr_in broadcastAddrF[UDPcount], broadcastAddrS[UDPcount];  /* Broadcast address */
    char *broadcastIP;                										/* IP broadcast address */
    unsigned short broadcastPortF[UDPcount], broadcastPortS[UDPcount];     	/* Server port */
    char *sendStringFirst, *sendStringSecond;                 				/* String to broadcast */
    int broadcastPermission = 1;          									/* Socket opt to set permission to broadcast */
    unsigned int sendStringLenF, sendStringLenS;       						/* Length of string to broadcast */

    //broadcast IP address
    broadcastIP = "127.0.0.1";
    //broadcast ports for get
    for(int i = 32002, j = 0; i < 32011; i++)
        broadcastPortF[j++] = i;
    //broadcast msg for get
    sendStringFirst = "I'm ready to get msg";
    //broadcast ports for send
    for(int i = 32014, j = 0; i < 32024; i++)
        broadcastPortS[j++] = i;
    //broadcast msg for send
    sendStringSecond = "I'm ready to send msg";

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

    sendStringLenF = strlen(sendStringFirst);  /* Find length of sendString */
    sendStringLenS = strlen(sendStringSecond);  /* Find length of sendString */

    //Create TCP sock
    /* Create socket for incoming connections */
    if ((TCPservSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&TCPServAddr, 0, sizeof(TCPServAddr));   /* Zero out structure */
    TCPServAddr.sin_family = AF_INET;                /* Internet address family */
    TCPServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    TCPServAddr.sin_port = htons(TCPServPortF);      /* Local port */

    /* Bind to the local address */
    if (bind(TCPservSock, (struct sockaddr *) &TCPServAddr, sizeof(TCPServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(TCPservSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    /* Set the size of the in-out parameter */
    clntLen = sizeof(TCPclntAddrF);

    printf("Server is working\n");

    for (;;) /* Run forever */
    {
        for(int i = 0; i < UDPcount; i++)
        {
            /* Broadcast sendString in datagram to clients every 5 seconds*/
            if (sendto(sockF[i], sendStringFirst, sendStringLenF, 0, (struct sockaddr *)
                       &broadcastAddrF[i], sizeof(broadcastAddrF[i])) != sendStringLenF)
                DieWithError("sendto() sent a different number of bytes than expected");

            if (sendto(sockS[i], sendStringSecond, sendStringLenS, 0, (struct sockaddr *)
                       &broadcastAddrS[i], sizeof(broadcastAddrS[i])) != sendStringLenS)
                DieWithError("sendto() sent a different number of bytes than expected");
        }

        /* Wait for a client to connect */
        if ((TCPsockF = accept(TCPservSock, (struct sockaddr *) &TCPclntAddrF,
                               &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(TCPclntAddrF.sin_addr));

        int recvMsgSize;

        msg = (struct mymsg*) malloc (sizeof(struct mymsg));

        if ((recvMsgSize = recv(TCPsockF, &msg, sizeof(*msg), 0)) < 0)
            DieWithError("recv() failed");

        printf("DATA:\n");
        printf("T=%d\n", msg->T);
        printf("N=%d\n", msg->N);
        printf("Message = %s", msg->text);

        sleep(5);   /* Avoids flooding the network */
    }
    /* NOT REACHED */
}
