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

#define MAX_SEND_SIZE 10

struct mymsgbuf {
    long mtype;
    char mtext[MAX_SEND_SIZE];
};

int msgqid, rc;

void send_message(int qid, struct mymsgbuf *qbuf, long type, char *text) {
    qbuf->mtype = type;
    strcpy(qbuf->mtext, text);

    if((msgsnd(qid, (struct msgbuf *)qbuf,
               strlen(qbuf->mtext)+1, 0)) ==-1) {
        perror("msgsnd");
        exit(1);
    }
}

int read_message(int qid, struct mymsgbuf *qbuf, long type) {
    qbuf->mtype = type;
    msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0);
    return atoi(qbuf->mtext);
}

int main(int argc, char *argv[]) {
    int i, status, stat;
    key_t key;
    int qtype = 1;
    struct mymsgbuf qbuf;
    short game = 1;
    short gamecount = 0;

    if (argc != 4) {
        printf("Usage: ./warcraft all_gold unit_count gold_per_unit\n");
        exit(-1);
    }

    int all_gold = atoi(argv[1]);
    int count = atoi(argv[2]), gpu = atoi(argv[3]);
    int pid[count];

    if(count < 1 || gpu < 1) {
        printf("Некоректное значение unit_count или gold_per_unit\n");
        exit(1);
    }

    if((msgqid = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1) {
        perror("msgget");
        exit(1);
    }

    while(game)
    {
        if(all_gold <= 0)
        {
            printf("Шахта уже пуста");
            return 0;
        }
        gamecount++;
        for (i = 1; i <= count; i++) {
            // запускаем дочерний процесс
            pid[i] = fork();
            srand(getpid());

            if (-1 == pid[i]) {
                perror("fork"); /* произошла ошибка */
                exit(1); /*выход из родительского процесса*/
            } else if (0 == pid[i]) {
                int vel;
                if(gpu * i - all_gold >= gpu)
                    vel = 0;
                else if(gpu * i - all_gold < gpu && gpu * i - all_gold > 0)
                {
                    vel = all_gold - gpu * (i - 1);
                }
                else
                {
                    vel = gpu;
                }
                printf("%d-ый юнит принимается за работу!\n", i);
                int time = rand() % 4;
                sleep(time);
                printf("Дело сделано! Идём на базу! %d-ый юнит закончил работу.\n", i);

                char str[10];
                sprintf(str, "%d", vel);
                send_message(msgqid, (struct mymsgbuf *)&qbuf, qtype, str);
                printf("%d-ый юнит добыл %d золота!\n", i, vel);
                fflush(stdout);
                exit(0); /* выход из процесс-потомока */
            }
        }
        // если выполняется родительский процесс
        // ожидание окончания выполнения всех запущенных процессов
        for (i = 0; i <= count; i++) {
            status = waitpid(pid[i], &stat, 0);
            if (pid[i] == status) {
                printf("Пришла информация. %d-ый юнит пришел на базу. (Результат процесса: %d)\n", i, WEXITSTATUS(stat));
                fflush(stdout);
            }
        }
        int wrk = 0;
        for (i = 1; i <= count; i++) {
            wrk += read_message(msgqid, &qbuf, qtype);
        }
        all_gold -= wrk;
        printf("\nВ шахте осталось %d золота\n", all_gold);
        if(!(gamecount % 3))
            printf("\nУ ваших юнитов повышение! Теперь они добывают %d золота\n", ++gpu);

    }
    if ((rc = msgctl(msgqid, IPC_RMID, NULL)) < 0) {
        perror( strerror(errno) );
        printf("msgctl (return queue) failed, rc=%d\n", rc);
        return 1;
    }
    return 0;
}
