#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#define Max_Size 256

int main (int argc, char *argv[])
{
    int a1, b1, c1;
    float a2, b2, c2;
    double a3, b3, c3;
    char choice;

    void *library_handler;
    int (*func_int)(int x, int y, int *z);
    float (*func_float)(float x, float y, float *z);
    double (*func_double)(double x, double y, double *z);

    library_handler = dlopen("./libfuncdyn.so",RTLD_LAZY);
    if (!library_handler) {
        fprintf(stderr,"dlopen() error: %s\n", dlerror());
        exit(1);
    };

    FILE *fp;

    if ((fp = fopen (argv[1], "r")) == NULL)
    {
        printf ("Не удается открыть файл.\n");
        exit (1);
    }

    char *buf = (char*)calloc(Max_Size, sizeof(char*));
    for(int i = 0; !feof(fp); i+=2)
    {
        fgets(buf, Max_Size, fp);
        if(i < 2)
        {
            a1 = atoi(buf);
            memset (buf, Max_Size, sizeof (char));
            fgets(buf, Max_Size, fp);
            b1 = atoi(buf);
            memset (buf, Max_Size, sizeof (char));
        }
        else if(i < 4)
        {
            a2 = atof(buf);
            memset (buf, Max_Size, sizeof (char));
            fgets(buf, Max_Size, fp);
            b2 = atof(buf);
            memset (buf, Max_Size, sizeof (char));
        }
        else
        {
            a3 = atof(buf);
            memset (buf, Max_Size, sizeof (char));
            fgets(buf, Max_Size, fp);
            b3 = atof(buf);
            memset (buf, Max_Size, sizeof (char));
            choice = fgetc(fp);
            break;
        }
    }

    fclose(fp);

    if (choice == '+')
    {
        func_int = dlsym(library_handler, "sum_int");
        (*func_int)(a1, b1, &c1);
        func_float = dlsym(library_handler, "sum_float");
        (*func_float)(a2, b2, &c2);
        func_double = dlsym(library_handler, "sum_double");
        (*func_double)(a3, b3, &c3);
    }
    else if (choice == '-')
    {
        func_int = dlsym(library_handler, "sub_int");
        (*func_int)(a1, b1, &c1);
        func_float = dlsym(library_handler, "sub_float");
        (*func_float)(a2, b2, &c2);
        func_double = dlsym(library_handler, "sub_double");
        (*func_double)(a3, b3, &c3);
    }
    else
    {
        printf ("Ошибка при указании операции\n");
        exit (1);
    }

    if ((fp = fopen (argv[1], "a")) == NULL)
    {
        printf ("Не удается открыть файл.\n");
        exit (1);
    }

    fprintf(fp, "\nres_i = %d, res_f = %f, res_d = %lf", c1, c2, c3);

    fclose(fp);

    dlclose(library_handler);

    return 0;
}
