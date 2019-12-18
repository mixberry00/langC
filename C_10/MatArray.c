#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#define MAX 100
#define MAXMAS 50

static int nitems;
struct {
    pthread_mutex_t	mutex;
    double results[MAXMAS];
    int	nput;
    double nval;
} shared = {
    PTHREAD_MUTEX_INITIALIZER
};

struct Array {
    int array[MAX];
    int n;
};

void *exp_value(void *arg) {
    sleep(rand()%5);
    while(1)
    {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return(NULL);
        }

        struct Array* a = (struct Array*)arg;
        double *res = (double*)malloc(sizeof(double));
        *res = 0.0;
        for(int i = 0; i < a->n; i++)
            *res += a->array[i];
        *res /= a->n;
        shared.nval = *res;
        shared.results[shared.nput] = shared.nval;
        shared.nput++;
        pthread_mutex_unlock(&shared.mutex);
        pthread_exit((void*)res);
        sleep(1);
    }
}

int main(int argc, char * argv[]) {
    int result;
    pthread_t threads[argc - 1];
    struct Array arr[argc - 1];
    void *status[argc - 1];
    FILE *fp;

    if(argc < 2)
    {
        perror("Usage <filename1> <filename2> ...\n");
        exit(1);
    }
    nitems = argc - 1;
    for (int i = 0; i < argc - 1; i++) {
        if((fp = fopen(argv[i + 1], "r")) == NULL)
        {
            perror("File does not exist\n");
            return EXIT_FAILURE;
        }
        arr[i].n = 0;
        for(int j = 0; !feof(fp); j++)
        {
            fscanf(fp, "%d ", &arr[i].array[j]);
            arr[i].n++;
        }
        result = pthread_create(&threads[i], NULL, exp_value, &arr[i]);

        if (result != 0) {
            perror("Creating the first thread");
            return EXIT_FAILURE;
        }
        if(fclose(fp)) {
            printf("File didn't close.\n");
            exit(1);
        }
    }

    for (int i = 0; i < argc - 1; i++) {
        result = pthread_join(threads[i], &status[i]);
        if (result != 0) {
            perror("Joining the first thread");
            return EXIT_FAILURE;
        } else {
            printf("Extended Value of %d array=%f\n", i, *((double*)status[i]));
        }
        free(status[i]);
    }

    printf("Mutex Results:\n");
    for(int i = 0; i < shared.nput; i++)
        printf("%f ", shared.results[i]);

    printf("\nDone..\n");
    return EXIT_SUCCESS;
}
