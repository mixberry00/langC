#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void func(FILE *fp, char *res, char *symbol, char *filename)
{
	if((fp=fopen(filename, "r"))==NULL) {
    printf("Не удается открыть файл.\n");
    exit(1);
	}
	
	char ch;
	int i = 0;	
	while((ch=fgetc(fp)) != EOF) 
		if(ch != *symbol)
			res[i++] = ch;
	
	if(fclose(fp)){ 
	printf("Ошибка при закрытии файла.\n");
	exit(1);
	}
}

void output(FILE *fp, char *res, char *filename)
{
	if((fp=fopen(filename, "w"))==NULL) {
    printf("Не удается открыть файл.\n");
    exit(1);
	}
	
	while(*res){ 
	if(!ferror(fp))
		fputc(*res++, fp);
	}
	
	if(fclose(fp)){ 
	printf("Ошибка при закрытии файла.\n");
	exit(1);
	}
}

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	char *res = (char*)calloc(256, sizeof(char*));
	
	if (argc < 3){
		fprintf (stderr, "Мало аргументов. Используйте <имя файла> <символ>\n");
		exit (1);
    }
    	
	func(fp, res, argv[2], argv[1]);
		
	output(fp, res, argv[1]);
	
	free(res);
	
	return 0;
}
