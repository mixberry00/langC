#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define LOWLIMIT 0
//Входные параметры: файл с координатами шарика
//Выходные параметры: -
int main (int argc, char *argv[])
{
    int x, y, res;
    char buf;
    if (argc < 2)
    {
        printf ("Usage: file textfile\n");
        exit (-1);
    }

    FILE *fp;

    if ((fp = fopen (argv[1], "r")) == NULL)
    {
        printf ("Не удается открыть файл.\n");
        exit (1);
    }

    fscanf (fp, "%d %d", &x, &y);

    close (fp);

    srand (time (NULL));

    x += rand () / RAND_MAX * (10 + 10) - 10;

    y += rand () / RAND_MAX * (10 + 10) - 10;

    if ((fp = fopen (argv[1], "w")) == NULL)
    {
        printf ("Не удается открыть файл.\n");
        exit (1);
    }

    fprintf (fp, "%d %d", x, y);
    
    if(x < LOWLIMIT || y < LOWLIMIT)
		res = 0;
	else
		res = 1;

    close (fp);
    return res;
}
