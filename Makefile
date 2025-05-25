CC=g++
CFLAGS=-I.
DEPS = picosha2.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: main.o
	$(CC) -o ../cge-multitool main.o