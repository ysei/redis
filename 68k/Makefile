INCLUDEDIR=-I..
ARFLAGS = cur
RANLIB = ranlib
SRC=68k_decode.c 68k_dis.c 68k_modes.c
OBJ=68k_decode.o 68k_dis.o 68k_modes.o
TARGET=lib68k.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)

depend:
	makedepend -Y -- $(CFLAGS) $(INCLUDEDIR) -- $(SRC) >/dev/null 2>&1
clean:
	rm -f *~ *.o $(TARGET)