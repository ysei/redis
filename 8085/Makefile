INCLUDEDIR=-I..
ARFLAGS = cur
RANLIB = ranlib
SRC=8085_dis.c
OBJ=8085_dis.o
TARGET=lib8085.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)

depend:
	makedepend -Y -- $(CFLAGS) $(INCLUDEDIR) -- $(SRC) >/dev/null 2>&1
clean:
	rm -f *~ *.o $(TARGET)

8085_dis.o: 8085_dis.h 8085_tables.h