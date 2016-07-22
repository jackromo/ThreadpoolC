
OBJECTS = $(patsubst %.c, %.o, $(shell ls *.c))

LDLIBS =
CFLAGS = -g -O3 -Wall -std=gnu11
CC = gcc

threadpool: $(OBJECTS)

clean:
	rm threadpool *.o
