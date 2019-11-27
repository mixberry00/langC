#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define LOWLIMIT 0

//Входные параметры: файл с координатами шарика
//Выходные параметры: -
int func(char *filename)
{
    int x, y, res;
    char buf;

    FILE *fp;

    if ((fp = fopen (filename, "r")) == NULL)
    {
        printf ("Не удается открыть файл.\n");
        exit (1);
    }

    fscanf (fp, "%d %d", &x, &y);

    fclose (fp);

    srand (time(NULL));

    x += rand () % (20 + 20 + 1) - 20;

    y += rand () % (20 + 20 + 1) - 20;

    if ((fp = fopen (filename, "w")) == NULL)
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
    exit(res);
}

// Входные параметры: список имен файлов для обработки 

// Выходные параметры: нет 
int main(int argc, char *argv[]) {
    int i, pid[argc], status, stat;
    char arg[20];
    // для всех файлов, перечисленных в командной строке 
    if (argc<2) {
        printf("Usage: file textfile1 textfile2 ...\n");
        exit(-1);
    }
    for (i = 1; i < argc; i++) { 
        // запускаем дочерний процесс 
        strcpy(arg,argv[i]);
        pid[i] = fork(); 
        if (pid[i] == 0) {
            // если выполняется дочерний процесс 
            // вызов функции рассчитывания координат            
            func(argv[i]);
        }
		// если выполняется родительский процесс
        printf("\nPARENT: Это процесс-родитель!\n");
		printf("PARENT: Мой PID -- %d\n", getpid());
		printf("PARENT: PID моего потомка %d\n",pid[i]);        
    }
    sleep(1);
    // ожидание окончания выполнения всех запущенных процессов
    for (i = 1; i< argc; i++) {         
        status=waitpid(pid[i],&stat,WNOHANG);
        if (pid[i] == status) {
            printf("\nFile %s done\n",argv[i]);
            if(WEXITSTATUS(stat))
				printf("Мячик не вышел за границы\n");
			else
				printf("Мячик вышел за границы\n");
			printf("Номер сигнала завершивший дочерний процесс - %d\n", WTERMSIG(stat));
        }
    }
    return 0; 
}
