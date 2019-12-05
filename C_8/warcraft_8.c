#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>

#define MAX_SEND_SIZE 80

struct mymsgbuf {
        long mtype;
        char mtext[MAX_SEND_SIZE];
};

int msgqid, rc;

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text){
        qbuf->mtype = type;
        strcpy(qbuf->mtext, text);

        if((msgsnd(qid, (struct msgbuf *)qbuf,
                strlen(qbuf->mtext)+1, 0)) ==-1){
                perror("msgsnd");
                exit(1);
        }
}

int read_message(int qid, struct mymsgbuf *qbuf, long type){
        qbuf->mtype = type;
        msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
        printf("Type: %ld Text: %s\n", qbuf->mtype, qbuf->mtext);
        return atoi(qbuf->mtext);
}

int main(int argc, char *argv[]) {
    int i, status, stat;
    key_t key;
    int qtype = 1;
    struct mymsgbuf qbuf;
    
    if (argc < 2) {
        printf("Usage: ./warcraft all_gold unit_count gold_per_unit\n");
        exit(-1);
    }
    
    int all_gold = atoi(argv[1]);
    int count = atoi(argv[2]), gpu = atoi(argv[3]);
    int pid[count];
    int fd[argc][2];
    
	if((msgqid = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1) {
		perror("msgget");
		exit(1);
	}
        
    for (i = 1; i < argc; i++) {
		// создаем канал
		pipe(fd[i]);
        // запускаем дочерний процесс 
        pid[i] = fork();
        srand(getpid());

        if (-1 == pid[i]) {
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
        } else if (0 == pid[i]) {
            close(fd[i][0]);
            int vel;
            if(all_gold == 0)
                vel = 0;
            else
            {
                if(all_gold < gpu)
                    vel = all_gold;
                else
                    vel = gpu;
                printf("Начинаем работу\n");
                int time = rand() % 4;
                sleep(time);
                printf("Заканчиваем работу!\n");

				char str[10];
				sprintf(str, "%d", vel);
                send_message(msgqid, (struct mymsgbuf *)&qbuf, qtype, str);
                printf(" CHILD: Это %d процесс-потомок отправил сообщение!\n", i);
                fflush(stdout);
            }
            int res = all_gold - vel;
            write(fd[i][1], &res, sizeof(int));
            exit(0); /* выход из процесс-потомока */
        }
    }
    // если выполняется родительский процесс
    printf("PARENT: Это процесс-родитель!\n");
    // ожидание окончания выполнения всех запущенных процессов
    for (i = 1; i < argc; i++) {
		close(fd[i][1]);
		read(fd[i][0], &all_gold, sizeof(int));
		printf("\n%d\n", all_gold);
        status = waitpid(pid[i], &stat, 0);
        if (pid[i] == status) {
            printf("процесс-потомок %d done,  result=%d\n", i, WEXITSTATUS(stat));
            fflush(stdout);
        }
    }
	int wrk = 0;
    for (i = 1; i < argc; i++) {
		wrk += read_message(msgqid, &qbuf, qtype); 
	}
	all_gold -= wrk;
	printf("Золота осталось %d\n", all_gold);
	
	if ((rc = msgctl(msgqid, IPC_RMID, NULL)) < 0) {
		perror( strerror(errno) );
		printf("msgctl (return queue) failed, rc=%d\n", rc);
		return 1;
	}
    return 0;
}