CC=gcc -std=gnu99

nsh: nsh.o parse.o

nsh.o: nsh.c parse.c parse.h

parse.o: parse.c parse.h

clean:
	rm -f *.o