# Project: nfsuserver
# Makefile created by Dev-C++ 4.9.5.0

CC   = g++
WINDRES = windres.exe
RES  = 
OBJ  = nfsuserver2.o objects.o server.o $(RES)
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

nfsuserver2.o: nfsuserver2.cpp
	$(CC) -c nfsuserver2.cpp -o nfsuserver2.o $(CFLAGS)

server.o: server.cpp
	$(CC) -c server.cpp -o server.o $(CFLAGS)

objects.o: objects.cpp
	$(CC) -c objects.cpp -o objects.o $(CFLAGS)
