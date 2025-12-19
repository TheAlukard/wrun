CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wno-absolute-value -Wno-sign-compare -pedantic
LFLAGS=-static -L lib/ -lm -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows
EXEC=./build/wrun
SRC=$(shell ls ./src/*.c)

all: release

release: $(SRC) ready
	$(CC) -o $(EXEC) $(SRC) -O3 $(CFLAGS) $(LFLAGS)

debug: $(SRC) ready
	$(CC) -o $(EXEC) $(SRC) -O0 -g $(CFLAGS) $(LFLAGS)

ready:
	$(shell mkdir build)

run: release
	$(shell $(EXEC))
