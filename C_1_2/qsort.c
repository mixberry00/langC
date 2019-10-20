#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 10

int count = 0;
int sm_size = 0;

static int cmpstringp(const void *p1, const void *p2)
{
	count++;
	return strlen(*(char * const*)p1) - strlen(*(char* const*)p2);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <string>...\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	qsort(&argv[1], argc - 1, sizeof(char *), cmpstringp);
	
	sm_size = strlen(argv[1]);
	
	for (int j = 1; j < argc; j++){
		puts(argv[j]);
	}
	printf("Количество перестановок = %d\n", count);
	printf("Длина наименьшей строки = %d\n", sm_size);
	exit(EXIT_SUCCESS);
}
