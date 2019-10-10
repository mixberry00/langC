hello_world:hello_world.o
	gcc hello_world.o -o hello_world
hello_world.o:hello_world.c
	gcc -c hello_world.c
clean_hlwrld:
	rm hello_world hello_world.o
qsort:qsort.o
	gcc qsort.o -o qsort
qsort.o:qsort.c
	gcc -c qsort.c
clean_qsort:
	rm qsort qsort.o
