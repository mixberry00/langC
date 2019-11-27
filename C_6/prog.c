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

    fclose (fp);

    srand (time(NULL));

    x += rand () % (20 + 20 + 1) - 20;

    y += rand () % (20 + 20 + 1) - 20;

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

    fclose (fp);
    printf("\nCHILD: Это процесс-потомок!\n");
	printf("CHILD: Мой PID -- %d\n", getpid());
	printf("CHILD: PID моего родителя -- %d\n", getppid());
    return res;
}
