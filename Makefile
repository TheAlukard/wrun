CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wno-sign-compare -pedantic
LFLAGS=-L lib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows
EXEC=./build/wrun.exe
SRC=$(shell ls ./src/*.c)

all: release

release: $(SRC) build
	$(CC) -o $(EXEC) $(SRC) -O3 $(CFLAGS) $(LFLAGS)

debug: $(SRC) build
	$(CC) -o $(EXEC) $(SRC) -O0 -g $(CFLAGS) $(LFLAGS)

build:
	$(shell mkdir build)

run: release
	$(shell $(EXEC))
