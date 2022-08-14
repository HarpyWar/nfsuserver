# Project: nfsuserver
# Makefile created by Dev-C++ 4.9.5.0

CC   = g++
WINDRES = windres.exe
RES  = 
OBJ  = nfsuserver.o objects.o server.o $(RES)
LIBS =  -L"/lib" -lpthread
INCS =  #-I "." 
BIN  = nfsuserver
CFLAGS = $(INCS) -s -O2 -fexpensive-optimizations

.PHONY: all all-before all-after clean clean-custom

all: all-before nfsuserver all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o "nfsuserver" $(LIBS) $(CFLAGS)

nfsuserver.o: nfsuserver.cpp
	$(CC) -c nfsuserver.cpp -o nfsuserver.o $(CFLAGS)

server.o: server.cpp
	$(CC) -c server.cpp -o server.o $(CFLAGS)

objects.o: objects.cpp
	$(CC) -c objects.cpp -o objects.o $(CFLAGS)
