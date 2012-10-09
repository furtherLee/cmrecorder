OBJS = main.o cmrecorder.o
CFLAGS = -Wall -O2
CC = gcc
LIBS = -lm

.phony: clean

all: cmrecorder

cmrecorder: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o cmrecorder

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f cmrecorder
	rm -f *~
