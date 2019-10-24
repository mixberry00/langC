#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define Max_Size 256

extern void sum_int (int a, int b, int *c);

extern void sub_int (int a, int b, int *c);

extern void sum_float (float a, float b, float *c);

extern void sub_float (float a, float b, float *c);

extern void sum_double (double a, double b, double *c);

extern void sub_double (double a, double b, double *c);


int main (int argc, char *argv[])
{
    int a1, b1, c1;
    float a2, b2, c2;
    double a3, b3, c3;
    char choice;
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
        sum_int (a1, b1, &c1);
        sum_float (a2, b2, &c2);
        sum_double (a3, b3, &c3);
    }
    else if (choice == '-')
    {
        sub_int (a1, b1, &c1);
        sub_float (a2, b2, &c2);
        sub_double (a3, b3, &c3);
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

    return 0;
}
