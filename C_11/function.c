#include "TCPEchoServer.h"

int Check(int x, int y, int c)
{
    int res = 0;
    switch(c)
    {
    case 1:
        for(int i = x; i > 0; i--)
            res += Field[i][y];
        break;
    case 2:
        for(int i = y; i < MAX; i++)
            res += Field[x][i];
        break;
    case 3:
        for(int i = x; i < MAX; i++)
            res += Field[i][y];
        break;
    case 4:
        for(int i = y; i > 0; i--)
            res += Field[x][i];
        break;
    }
    return res;
}

int GameFunction(int x, int y)
{
    int res = 0;
    printf("%d %d\n", Field[0][0], Field[49][49]);
    if(x <= MAX/2 && y < MAX/2)
        if(x < y)
            res = Check(x,y,3);
        else
            res = Check(x,y,2);
    else if(x >= MAX/2 && y >= MAX/2)
        if(x < y)
            res = Check(x,y,4);
        else
            res = Check(x,y,1);
    else if(x <= MAX/2)
        if(x < y + MAX/2)
            res = Check(x,y,1);
        else
            res = Check(x,y,2);
    else if(x + MAX/2 < y)
        res = Check(x,y,4);
    else
        res = Check(x,y,3);
    return res;
}
