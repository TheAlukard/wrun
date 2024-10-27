CC=gcc
CFLAGS=-O3 -g -Wall -Wextra -Werror -pedantic
LFLAGS=-L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows
EXEC=./build/wrun.exe
SRC=$(shell ls ./src/*.c)

$(EXEC): $(SRC) build
	$(CC) -o $(EXEC) $(SRC) $(CFLAGS) $(LFLAGS)

build:
	$(shell mkdir build)
