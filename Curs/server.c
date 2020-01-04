#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sockF, sockS;                         /* Socket */
    struct sockaddr_in broadcastAddrF, broadcastAddrS; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    unsigned short broadcastPortF, broadcastPortS;     /* Server port */
    char *sendStringFirst, *sendStringSecond;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLenF, sendStringLenS;       /* Length of string to broadcast */

    broadcastIP = "127.0.0.1";            /* First arg:  broadcast IP address */ 
    broadcastPortF = 32002;    /* Second arg:  broadcast port */
    sendStringFirst = "I'm ready to get msg";             /* Third arg:  string to broadcast */
    broadcastPortS = 32004;    /* Second arg:  broadcast port */
    sendStringSecond = "I'm ready to send msg";    

    /* Create socket for sending/receiving datagrams */
    if ((sockF = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
        
    /* Create socket for sending/receiving datagrams */
    if ((sockS = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sockF, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");
        
    if (setsockopt(sockS, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");

    /* Construct local address structure */
    memset(&broadcastAddrF, 0, sizeof(broadcastAddrF));   /* Zero out structure */
    broadcastAddrF.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddrF.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
    broadcastAddrF.sin_port = htons(broadcastPortF);         /* Broadcast port */
    
    memset(&broadcastAddrS, 0, sizeof(broadcastAddrS));   /* Zero out structure */
    broadcastAddrS.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddrS.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
    broadcastAddrS.sin_port = htons(broadcastPortS);         /* Broadcast port */

    sendStringLenF = strlen(sendStringFirst);  /* Find length of sendString */
    sendStringLenS = strlen(sendStringSecond);  /* Find length of sendString */
    
    for (;;) /* Run forever */
    {
         /* Broadcast sendString in datagram to clients every 3 seconds*/
         if (sendto(sockF, sendStringFirst, sendStringLenF, 0, (struct sockaddr *) 
               &broadcastAddrF, sizeof(broadcastAddrF)) != sendStringLenF)
             DieWithError("sendto() sent a different number of bytes than expected");
             
		if (sendto(sockS, sendStringSecond, sendStringLenS, 0, (struct sockaddr *) 
               &broadcastAddrS, sizeof(broadcastAddrS)) != sendStringLenS)
             DieWithError("sendto() sent a different number of bytes than expected");

        sleep(5);   /* Avoids flooding the network */
    }
    /* NOT REACHED */
}
