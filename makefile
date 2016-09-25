
CFLAGS=-Wall -g

all: procsched

libs: lib_fichero.o lib_cola.o fragmenta.o

procsched: fragmenta.o lib_fichero.o procsched.c lib_cola.o
	gcc $(CFLAGS) procsched.c -o procsched lib_fichero.o fragmenta.o lib_cola.o

fragmenta.o: fragmenta.c
	gcc $(CFLAGS) -c fragmenta.c

lib_fichero.o: lib_fichero.c
	gcc $(CFLAGS) -c lib_fichero.c

lib_cola.o: lib_cola.c
	gcc $(CFLAGS) -c lib_cola.c

clean:
	rm *.o procsched 
