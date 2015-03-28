INCLUDEDIR=-I..
ARFLAGS = cur
RANLIB = ranlib
SRC=6809_dis.c
OBJ=6809_dis.o
TARGET=lib6809.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)

depend:
	makedepend -Y -- $(CFLAGS) $(INCLUDEDIR) -- $(SRC) >/dev/null 2>&1
clean:
	rm -f *~ *.o $(TARGET)