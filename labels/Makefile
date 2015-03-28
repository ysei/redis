CC = gcc
LD = gcc
RM = rm

#CFLAGS = -g -Wall -O0 -DOUTPUT
#LDFLAGS = -lefence -lm

CFLAGS = -O3 -g -Wall -DDEBUG
#CFLAGS = -O3
LDFLAGS = -lm

all:	parser flattener

parser:	tools.o labels.o
	$(LD) -o parser $(LDFLAGS) $^

flattener: tools.o flattener.o
	$(LD) -o flattener $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f parser flattener *.o *~

flattener.o:	flattener.h tools.h
labels.o:	tools.h labels.h
tools.o: 	tools.h
