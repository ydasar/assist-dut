all: assist-server dut-client

IDIR =./include
CC=gcc -g
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

$(shell mkdir -p $(ODIR))

_DEPS = assist.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = assist-server.o communication.o services.o services-utilities.o dut-client.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -g -Wall -Wextra -c -o $@ $< $(CFLAGS)

assist-server: $(OBJ)
	$(CC) -g -Wall -Wextra -o $@ assist-server.c communication.c services.c services-utilities.c $(CFLAGS) $(LIBS)

dut-client: $(OBJ)
	$(CC) -g -Wall -Wextra -o $@ dut-client.c communication.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ assist-server dut-client