#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>

#define MAX_LEN 1024
union semun {
    int val;                  /* значение для SETVAL */
    struct semid_ds *buf;     /* буферы для  IPC_STAT, IPC_SET */
    unsigned short *array;    /* массивы для GETALL, SETALL */
    /* часть, особенная для Linux: */
    struct seminfo *__buf;    /* буфер для IPC_INFO */
};

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: ./sem <Allhoney> <Whinnie power> <Count of bees> <Bees power>n");
        exit(-1);
    }
    int Allhoney = atoi(argv[1]), WhPwr = atoi(argv[2]),
        count = atoi(argv[3]), BPwr = atoi(argv[4]);

    pid_t pid[count];
    pid_t wpid;
    int status = 0;

    int shmid;
    int *shm;

    int semid;
    union semun arg;
    struct sembuf lock_res = {0, -1, 0};	//блокировка ресурса
    struct sembuf rel_res = {0, 1, 0};	//освобождение ресурса


    /* Создадим семафор - для синхронизации работы с разделяемой памятью.*/
    semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);

    /* Установить в семафоре № 0 (Контроллер ресурса) значение "1" */
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    /* Создадим область разделяемой памяти */
    if ((shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /* Получим доступ к разделяемой памяти */
    if ((shm = (int*)shmat(shmid, NULL, 0)) == (int *) -1) {
        perror("shmat");
        exit(1);
    }

    *(shm) = Allhoney;

    if (shmdt(shm) < 0) {
        printf("Ошибка отключения\n");
        exit(1);
    }

    while(1)
    {
        for (int i = 0; i < count; i++)
        {
            pid[i] = fork();
            srand(getpid());
            if (pid[i] == 0)
            {
                printf("PID=%d i=%d\n", getpid(), i);
                fflush(stdout);

                /* Получим доступ к разделяемой памяти */
                if ((shm = (int *)shmat(shmid, NULL, 0)) == (int *) -1) {
                    perror("shmat");
                    exit(1);
                }

                printf("Процесс ожидает PID=%d i=%d\n", getpid(), i);
                fflush(stdout);

                /* Заблокируем разделяемую память */
                if((semop(semid, &lock_res, 1)) == -1) {
                    fprintf(stderr, "Lock failed\n");
                    exit(1);
                } else {
                    printf("Semaphore resources decremented by one (locked) i=%d\n", i);
                    fflush(stdout);
                }

                /* Запишем в разделяемую память сумму */
                *(shm) = *(shm) + BPwr;
                sleep(rand() % 4);

                /* Освободим разделяемую память */
                if((semop(semid, &rel_res, 1)) == -1) {
                    fprintf(stderr, "Unlock failed\n");
                    exit(1);
                } else {
                    printf("Semaphore resources incremented by one (unlocked) i=%d\n", i);
                    fflush(stdout);
                }

                printf("AllhoneyNew = %d\n", *(shm));
                fflush(stdout);

                /* Отключимся от разделяемой памяти */
                if (shmdt(shm) < 0) {
                    printf("Ошибка отключения\n");
                    exit(1);
                }
                exit(0);
            }
            else if (pid[i] < 0)
            {
                perror("fork"); /* произошла ошибка */
                exit(1); /*выход из родительского процесса*/
            }
        }

        for (int i = 0; i < count; i++) {
            wpid = waitpid(pid[i], &status, 0);
            if (pid[i] == wpid) {
                printf("процесс-потомок %d done,  result=%d\n", i+1, WEXITSTATUS(status));
                fflush(stdout);
            }
        }

        /* Получим доступ к разделяемой памяти */
        if ((shm = (int*)shmat(shmid, NULL, 0)) == (int *) -1) {
            perror("shmat");
            exit(1);
        }

        printf("Стало %d мёда\n", *(shm));
        if(*(shm) < WhPwr)
            printf("Мёда слишком мало для Винни\n");
        else
        {
            *(shm) = *(shm) - WhPwr;
            printf("После того как мёд подъел медведь стало %d мёда\n", *(shm));
        }
        sleep(5);

        if (shmdt(shm) < 0) {
            printf("Ошибка отключения\n");
            exit(1);
        }
    }

    /* Удалим созданные объекты IPC */
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("Невозможно удалить область\n");
        exit(1);
    } else
        printf("Сегмент памяти помечен для удаления\n");

    if (semctl(semid, 0, IPC_RMID) < 0) {
        printf("Невозможно удалить семафор\n");
        exit(1);
    }

    return 0;
}
