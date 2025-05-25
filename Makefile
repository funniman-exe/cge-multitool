CC=g++
CFLAGS=-I.
DEPS = thirdparty/picosha2.h thirdparty/json.hpp commands.h fastdl.h globs.h
OBJ = main.o commands.o fastdl.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cge-multitool: $(OBJ)
	$(CC) -o ../$@ $^ $(CFLAGS)