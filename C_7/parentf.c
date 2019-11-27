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
//Выходные параметры: результат перемещения шарика
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

    srand (getpid());

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
    return(res);
}

// Входные параметры: список имен файлов для обработки
// Выходные параметры: нет
int main(int argc, char *argv[]) {
    int i, pid[argc], status, stat;

    // для всех файлов, перечисленных в командной строке
    if (argc<2) {
        printf("Usage: file textfile1 textfile2 ...\n");
        exit(-1);
    }
    int fd[argc][2];
    for (i = 1; i < argc; i++) {
		// создаем канал
		pipe(fd[i]);
        // запускаем дочерний процесс
        pid[i] = fork();

        if (-1 == pid[i]) {
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
        } else if (pid[i] == 0) {
            // если выполняется дочерний процесс
            // процесс-потомок закрывает доступный для чтения конец канала 0
            close(fd[i][0]);
            // вызов функции рассчитывания координат и запись в канал 
            int res = func(argv[i]);
            write(fd[i][1], &res, sizeof(int));
            exit(0);
        }
    }
    printf("\nPARENT: Это процесс-родитель!\n");
    for (i = 1; i < argc; i++) {
        status = waitpid(pid[i],&stat,0);
        if (pid[i] == status) {
            printf("\nFile %s done, result is %d\n",argv[i], WEXITSTATUS(stat));
            // процесс-родитель закрывает доступный для записи конец канала 1
            close(fd[i][1]);
            int res = 0;
            //считываем из канала 0
            read(fd[i][0], &res, sizeof(int));
            if(res)
                printf("Мячик не вышел за границы\n");
            else
                printf("Мячик вышел за границы\n");
        }
    }
    return 0;
}
