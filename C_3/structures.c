#include <stdio.h>
#include <stdlib.h>
struct Person
{
	int year;
	int number;
	int salary;
};

typedef struct Person employee;

void read_employee(employee *st)
{
	printf("\nВведите год рождения отрудника:\n");
	scanf("%d", &st->year);
	printf("Введите номер отдела сотрудника:\n");
	scanf("%d", &st->number);
	printf("Введите оклад сотрудника:\n");
	scanf("%d", &st->salary);
}

static int cmpstruct(const void *p2, const void *p1)
{
	employee * st1 = *(employee**)p1;
    employee * st2 = *(employee**)p2;
    return st2->year - st1->year;
}

int main(int argc, char *argv[])
{
	int count = 3;
    printf("Введите кол-во сотрудников:");
    scanf("%d", &count);
	employee** st = (employee**)malloc(sizeof(employee*)*count);
	for(int i = 0; i < count; i++)
	{
		printf("Введите данные %d-го сотрудника", i+1);
		st[i] = (employee*) malloc (sizeof(employee));
		read_employee(st[i]);
	}
	
	qsort(st, count, sizeof(struct employee*), cmpstruct);
	
	for(int i = 0; i < count; i++)
	{
		printf("%d-сотрудник: Год:%d, Номер отдела:%d Оклад:%d\n", i, st[i]->year, st[i]->number, st[i]->salary);
	}
	
	for (int i = 0; i < count; i++)
    {
        free(st[i]);
    }
    free(st);
    return 0;
}
