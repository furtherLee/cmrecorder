OBJS = main.o cmrecorder.o
CFLAGS = -Wall -O2
CC = gcc

.phony: clean

all: cmrecorder

cmrecorder: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cmrecorder

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f cmrecorder
	rm -f *~
