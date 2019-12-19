#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <unistd.h>     /* for close() */
#include "protocol.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */
//function for calculating number
//of people on the field
int GameFunction(int x, int y);

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    struct protocol proto;
    if ((recvMsgSize = recv(clntSocket, &proto, sizeof(proto), 0)) < 0)
        DieWithError("recv() failed");

    printf("Client coordinates:\n");
    printf("x=%d\n", proto.x);
    printf("y=%d\n", proto.y);

    int res = GameFunction(proto.x, proto.y);
    printf("res=%d\n", res);

    if (send(clntSocket, &res, sizeof(int), 0) != sizeof(int))
        DieWithError("send() failed");

    fflush(stdout);
    close(clntSocket);    /* Close client socket */
}
