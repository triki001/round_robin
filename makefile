
CFLAGS=-Wall -g

all: procsched

libs: parser.o queue.o tokenizer.o

procsched: tokenizer.o parser.o procsched.c queue.o
	gcc $(CFLAGS) procsched.c -o procsched parser.o tokenizer.o queue.o

tokenizer.o: tokenizer.c
	gcc $(CFLAGS) -c tokenizer.c

parser.o: parser.c
	gcc $(CFLAGS) -c parser.c

queue.o: queue.c
	gcc $(CFLAGS) -c queue.c

clean:
	rm *.o procsched 
