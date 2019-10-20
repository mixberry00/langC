#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	FILE *ip, *op;
	char *input = (char*)calloc(256, sizeof(char*));
	int len_lim = 0;
	if (argc < 3){
		fprintf (stderr, "Мало аргументов. Используйте <имя файла> <заданная длина строки>\n");
		exit (1);
    }
    	
	if((ip=fopen(argv[1], "r"))==NULL) {
    printf("Не удается открыть файл.\n");
    exit(1);
	}
	
	len_lim = atoi(argv[2]);
	
	char *fname = argv[1];
	strcat(argv[1], ".txt");	
	
	if((op=fopen(fname, "w"))==NULL) {
    printf("Не удается открыть файл.\n");
    exit(1);
	}
		
	while(!feof(ip))
	{
		fscanf(ip,"%s[]", input);
		if(strlen(input) >= len_lim)
			fprintf(op,"%s ", input);
		memset(input, 256, sizeof(char));
	}
	
	if(fclose(ip)){ 
	printf("Ошибка при закрытии файла.\n");
	exit(1);
    }
	
	if(fclose(op)){ 
	printf("Ошибка при закрытии файла.\n");
	exit(1);
	}
	
	free(input);
	
	return 0;
}
