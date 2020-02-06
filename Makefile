all: assist-server dut-client

DIR := ${CURDIR}
IDIR =${DIR}/include
CC=gcc
#CC=/opt/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS=-I$(IDIR)

ODIR=obj

$(shell mkdir -p $(ODIR))

_DEPS = assist.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = assist-server.o communication.o services.o services-utilities.o dut-client.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -Wextra -c -o $@ $< $(CFLAGS)

assist-server: $(OBJ)
	$(CC) -Wall -Wextra -o $@ assist-server.c communication.c services.c services-utilities.c $(CFLAGS) $(LIBS)

dut-client: $(OBJ)
	$(CC) -Wall -Wextra -o $@ dut-client.c communication.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/ *~ core $(INCDIR)/*~ assist-server dut-client
