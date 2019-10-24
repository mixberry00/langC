statlib_use: statlib_use.o libfunc.a
	gcc statlib_use.o -o statlib_use -Wall -L. -lfunc
statlib_use.o:statlib_use.c
	gcc -c statlib_use.c
libfunc.a: func.o
	ar rc libfunc.a func.o
	ranlib libfunc.a
func.o: func.c
	gcc -c func.c
clean_stat:
	rm libfunc.a func.o statlib_use statlib_use.o
	
dynlib_use: libfuncdyn.so dynlib_use.o
	gcc -o dynlib_use dynlib_use.o -L. -lfuncdyn -Wl,-rpath,.
dynamic_prog: libfuncdyn.so dynamic_prog.o
	gcc -o dynamic_prog dynamic_prog.o -ldl

dynamic_prog.o: dynamic_prog.c
	gcc -c dynamic_prog.c
dynlib_use.o:dynlib_use.c
	gcc -c dynlib_use.c
libfuncdyn.so:funcdyn.o
	gcc -shared -o libfuncdyn.so funcdyn.o
funcdyn.o:funcdyn.c
	gcc -c -fPIC funcdyn.c
clean_dynlib:
	rm dynlib_use dynlib_use.o funcdyn.o libfuncdyn.so 
clean_dynamic_prog:
	rm dynamic_prog dynamic_prog.o
