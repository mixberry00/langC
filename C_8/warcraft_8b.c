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

int msgqid, rc, all_gold;

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text){
        qbuf->mtype = type;
        strcpy(qbuf->mtext, text);
        if((msgsnd(qid, (struct msgbuf *)qbuf,
                strlen(qbuf->mtext)+1, 0)) ==-1){
                perror("msgsnd");
                exit(1);
        }
}

void read_message(int qid, struct mymsgbuf *qbuf, long type) {
    qbuf->mtype = type;
    msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
    printf("Я добыл %s золота\n", qbuf->mtext);
}

int main(int argc, char *argv[]) {
    int i, status, stat;
    key_t key;
    int qtype = 1;
    struct mymsgbuf qbuf;

    if (argc != 4)
    {
        printf("Usage: ./warcraft all_gold unit_count gold_per_unit\n");
        exit(-1);
    }

    all_gold = atoi(argv[1]);
    int count = atoi(argv[2]), gpu = atoi(argv[3]);
    int pid[count];

    if((msgqid = msgget(IPC_PRIVATE, IPC_CREAT|0660) == -1))
    {
        perror("msgget");
        exit(1);
    }
    printf("Золота в шахте: %d\n", all_gold);
    for(i = 1; i <= count; i++)	//&& all_gold > 0
    {
        // запускаем дочерний процесс
        pid[i] = fork();
        srand(getpid());

        if (-1 == pid[i])
        {
            perror("fork"); /* произошла ошибка */
            exit(1); /*выход из родительского процесса*/
        }
        else if (0 == pid[i])
        {
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
				sprintf(str, "%d", (int)strlen(argv[i]));
                send_message(msgqid, (struct mymsgbuf *)&qbuf, qtype, str);
                printf(" CHILD: Это %d процесс-потомок отправил сообщение!\n", i);
                fflush(stdout);
            }
            all_gold -= vel;
            exit(0); /* выход из процесс-потомока */
        }
    }
    // если выполняется родительский процесс
    printf("PARENT: Это процесс-родитель!\n");
    // ожидание окончания выполнения всех запущенных процессов
    for (i = 1; i < argc; i++)
    {
        status = waitpid(pid[i], &stat, 0);
        if (pid[i] == status)
        {
            printf("процесс-потомок %d done,  result=%d\n", i, WEXITSTATUS(stat));
            fflush(stdout);
        }
    }
    printf("Золота в шахте после посещения: %d\n", all_gold);
    for (i = 1; i < argc; i++)
    {
        read_message(msgqid, &qbuf, qtype);
    }

    if ((rc = msgctl(msgqid, IPC_RMID, NULL)) < 0)
    {
        perror(strerror(errno));
        printf("msgctl (return queue) failed, rc=%d\n", rc);
        return 1;
    }
    return 0;
}
