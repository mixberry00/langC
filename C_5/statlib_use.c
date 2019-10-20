#include <stdio.h>

int main(int argc, char *argv[])
{
	int a1, b1, c1;
	float a2, b2, c2;
	double a3, b3, c3;
	char choice;
	
	printf("Введите два int числа:\n");
	scanf("%d %d", &a1, &b1);
	printf("Введите два float числа:\n");
	scanf("%f %f", &a2, &b2);
	printf("Введите два double числа:\n");
	scanf("%lf %lf", &a3, &b3);
	printf("Какую операцию выполнить?(+ -)\n");
	scanf("%s", &choice);
	printf("\n%c\n", choice);
	if(choice == '+')
	{
		c1 = sum_int(&a1, &b1);
		c2 = sum_float(&a2, &b2);
		c3 = sum_double(&a3, &b3);
	}
	else if(choice == '-')
	{
		c1 = sub_int(&a1, &b1);
		c2 = sub_float(&a2, &b2);
		c3 = sub_double(&a3, &b3);
	}
	else
	{
		printf("Ошибка при указании операции\n");
		exit(1);
	}
	printf("%d %f %lf", c1, c2, c3);
	return 0;
}
