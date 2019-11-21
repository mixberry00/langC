parent: parent.o prog.o parentf.o
	gcc parent.o -o parent
	gcc prog.o -o prog
	gcc parentf.o -o parentf
parent.o: parent.c
	gcc -c parent.c
prog.o: prog.c
	gcc -c prog.c
parentf.o: parentf.c
	gcc -c parentf.c
clean:
	rm parent.o parent prog.o prog parentf.o parentf
