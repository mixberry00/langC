#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
            if (execl("./prog","prog",arg, NULL) < 0) {
                printf("ERROR while start processing file %s\n",argv[i]);
                exit(-2);
            }
            else printf( "processing of file %s started (pid=%d)\n", argv[i],pid[i]);
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
            printf("File %s done\n",argv[i]);
            if(WEXITSTATUS(stat))
				printf("Мячик не вышел за границы\n");
			else
				printf("Мячик вышел за границы\n");
			printf("Номер сигнала завершивший дочерний процесс - %d\n", WTERMSIG(stat));
        }
    }
    return 0; 
}
