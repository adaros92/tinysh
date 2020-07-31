CFLAGS= -Wall -lm -std=gnu99

cli.o: cli.h cli.c
	gcc -c cli.c -g $(CFLAGS)

util.o: util.h util.c
	gcc -c util.c -g $(CFLAGS)

shell.o: util.o shell.h shell.c
	gcc -c shell.c -g util.o $(CFLAGS)

smallsh: util.o cli.o shell.o smallsh.c
	gcc -o smallsh smallsh.c -g cli.o shell.o util.o $(CFLAGS)

all: smallsh

clean:
	rm -rf smallsh *.o *.dSYM junk junk2 mytestresults