CC = gcc
LD = gcc

#CFLAGS = -Wall -g -DDEBUG
CFLAGS = -Wall -g
LDFLAGS= 

SRC=configfile.cpp  discore.cpp  dump.cpp  labels.cpp  main.cpp  strextractor.cpp  textractor.cpp modes.cpp
OBJ=configfile.o  discore.o  dump.o  labels.o  main.o  strextractor.o  textractor.o modes.o
EXTRAMODS=68k/lib68k.a 6809/lib6809.a 8085/lib8085.a
SUBDIRS=68k 6809 8085

all:	libs redis

libs:
	for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

redis:  $(OBJ) $(EXTRAMODS)
	$(LD) -o redis  $(LDFLAGS)   $^ $(EXTRAMODS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<
depend:
	makedepend -Y  -- $(CFLAGS) -- $(SRC) >/dev/null 2>&1
clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
	rm -rf *~ *.o redis
